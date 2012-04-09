/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtAddOn.JsonDb module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qjsondbprivatepartition_p.h"
#include "qjsondbconnection_p.h"
#include "qjsondbrequest_p.h"
#include "qjsondbstrings_p.h"

#include "jsondbowner.h"
#include "jsondbpartition.h"
#include "jsondbquery.h"

#include <qdir.h>
#include <pwd.h>

QT_BEGIN_NAMESPACE_JSONDB

QJsonDbPrivatePartition::QJsonDbPrivatePartition(QJsonDbConnectionPrivate *conn) :
    connection(conn), partitionOwner(0), privatePartition(0)
{
}

QJsonDbPrivatePartition::~QJsonDbPrivatePartition()
{
    if (privatePartition) {
        privatePartition->close();
        delete privatePartition;
        privatePartition = 0;
    }

    if (partitionOwner) {
        delete partitionOwner;
        partitionOwner = 0;
    }
}

void QJsonDbPrivatePartition::handleRequest()
{
    QString partitionName = request.value(JsonDbStrings::Protocol::partition()).toString();

    QString errorMessage;
    QJsonDbRequest::ErrorCode errorCode = ensurePartition(partitionName, errorMessage);

    if (errorCode == QJsonDbRequest::NoError) {
        QList<QJsonObject> results;

        if (request.value(JsonDbStrings::Protocol::action()).toString() == JsonDbStrings::Protocol::update()) {
            Partition::JsonDbObjectList objects;
            QJsonArray objectArray = request.value(JsonDbStrings::Protocol::object()).toArray();
            if (objectArray.isEmpty())
                objectArray.append(request.value(JsonDbStrings::Protocol::object()).toObject());

            foreach (const QJsonValue &val, objectArray)
                objects.append(val.toObject());

            Partition::JsonDbPartition::WriteMode writeMode =
                    request.value(JsonDbStrings::Protocol::conflictResolutionMode()).toString() == JsonDbStrings::Protocol::replace() ?
                        Partition::JsonDbPartition::ForcedWrite : Partition::JsonDbPartition::OptimisticWrite;
            Partition::JsonDbWriteResult writeResults = privatePartition->updateObjects(privatePartition->defaultOwner(), objects, writeMode);
            if (writeResults.code == Partition::JsonDbError::NoError) {
                emit writeRequestStarted(writeResults.state);
                emit statusChanged(QJsonDbRequest::Receiving);
                emit started();

                results.reserve(writeResults.objectsWritten.count());
                foreach (const Partition::JsonDbObject &object, writeResults.objectsWritten) {
                    QJsonObject written;
                    written.insert(JsonDbStrings::Property::uuid(), object.uuid().toString());
                    written.insert(JsonDbStrings::Property::version(), object.version());
                    results.append(written);
                }

                emit resultsAvailable(results);
            } else {
                errorCode = static_cast<QJsonDbRequest::ErrorCode>(writeResults.code);
                errorMessage = writeResults.message;
            }
        } else {
            Q_ASSERT(request.value(JsonDbStrings::Protocol::action()).toString() == JsonDbStrings::Protocol::query());
            QJsonObject object = request.value(JsonDbStrings::Protocol::object()).toObject();
            QScopedPointer<Partition::JsonDbQuery> query(Partition::JsonDbQuery::parse(
                                                             object.value(JsonDbStrings::Property::query()).toString(),
                                                             object.value(JsonDbStrings::Property::bindings()).toObject()));
            int limit = -1;
            int offset = 0;

            if (request.contains(JsonDbStrings::Property::queryLimit()))
                limit = request.value(JsonDbStrings::Property::queryLimit()).toDouble();
            if (request.contains(JsonDbStrings::Property::queryOffset()))
                offset = request.value(JsonDbStrings::Property::queryOffset()).toDouble();

            Partition::JsonDbQueryResult queryResult = privatePartition->queryObjects(privatePartition->defaultOwner(),
                                                                                      query.data(), limit, offset);
            if (queryResult.error.isNull()) {
                emit readRequestStarted(static_cast<quint32>(queryResult.state.toDouble()),
                                        queryResult.sortKeys.toArray().at(0).toString());
                emit statusChanged(QJsonDbRequest::Receiving);
                emit started();

                results.reserve(queryResult.data.count());
                foreach (const Partition::JsonDbObject &val, queryResult.data)
                    results.append(val);
                emit resultsAvailable(results);
            } else {
                QJsonObject error = queryResult.error.toObject();
                errorCode = static_cast<QJsonDbRequest::ErrorCode>(error.value(JsonDbStrings::Protocol::errorCode()).toDouble());
                errorMessage = error.value(JsonDbStrings::Protocol::errorMessage()).toString();
            }
        }
    }

    if (errorCode == QJsonDbRequest::NoError) {
        emit statusChanged(QJsonDbRequest::Finished);
        emit finished();
    } else {
        emit statusChanged(QJsonDbRequest::Error);
        emit error(errorCode, errorMessage);
    }

    emit requestCompleted();
}

QtJsonDb::QJsonDbRequest::ErrorCode QJsonDbPrivatePartition::ensurePartition(const QString &partitionName, QString &message)
{
    QString partition = partitionName == JsonDbStrings::Partition::privatePartition() ?
                QString::fromLatin1("%1.%2").arg(QString::fromLatin1(qgetenv("USER"))).arg(JsonDbStrings::Partition::privatePartition()) :
                partitionName;
    QString user = QString(partition).replace(QString::fromLatin1(".%1").arg(JsonDbStrings::Partition::privatePartition()), QStringLiteral(""));

    // only keep a single private partition open at a time. This will cut down
    // on file contention and also keep the memory usage of the client under control
    if (privatePartition && privatePartition->name() != partition) {
        privatePartition->close();
        delete privatePartition;
        privatePartition = 0;
    }

    if (!privatePartition) {
        struct passwd *pwd = getpwnam(user.toAscii());
        if (pwd) {
            if (!partitionOwner) {
                partitionOwner = new Partition::JsonDbOwner();
                partitionOwner->setAllowAll(true);
            }

            QDir homeDir(QString::fromUtf8(pwd->pw_dir));
            homeDir.mkdir(QStringLiteral(".jsondb"));
            homeDir.cd(QStringLiteral(".jsondb"));

            privatePartition = new Partition::JsonDbPartition(homeDir.absoluteFilePath(partition), partition, partitionOwner);
            privatePartition->setObjectName(QStringLiteral("private"));

            if (!privatePartition->open()) {
                message = QStringLiteral("Unable to open private partition");
                return QJsonDbRequest::InvalidPartition;
            }
        } else {
            message = QStringLiteral("Private partition not found");
            return QJsonDbRequest::InvalidPartition;
        }
    }

    return QJsonDbRequest::NoError;
}

#include "moc_qjsondbprivatepartition_p.cpp"

QT_END_NAMESPACE_JSONDB
