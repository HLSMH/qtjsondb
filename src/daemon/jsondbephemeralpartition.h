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

#ifndef JSONDB_EPHEMERAL_PARTITION_H
#define JSONDB_EPHEMERAL_PARTITION_H

#include <QUuid>
#include <QMap>
#include <QObject>
#include <qjsonobject.h>
#include "jsondbnotification.h"
#include "jsondbobject.h"
#include "jsondbpartition.h"
#include "jsondbquery.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_JSONDB_PARTITION
class JsonDbQuery;
QT_END_NAMESPACE_JSONDB_PARTITION

QT_USE_NAMESPACE_JSONDB_PARTITION

class JsonDbEphemeralPartition : public QObject
{
    Q_OBJECT
public:
    JsonDbEphemeralPartition(const QString &name, QObject *parent = 0);

    bool get(const QUuid &uuid, JsonDbObject *result) const;

    JsonDbQueryResult queryObjects(const JsonDbOwner *owner, const JsonDbQuery &query, int limit=-1, int offset=0);
    JsonDbWriteResult updateObjects(const JsonDbOwner *owner, const JsonDbObjectList &objects, JsonDbPartition::ConflictResolutionMode mode);

    void addNotification(JsonDbNotification *notification);
    void removeNotification(JsonDbNotification *notification);

    inline QString name() const { return mName; }

private Q_SLOTS:
    void objectsUpdated(const JsonDbUpdateList &changes);

private:
    typedef QMap<QUuid, JsonDbObject> ObjectMap;
    ObjectMap mObjects;
    QString mName;
    QMultiHash<QString, QPointer<JsonDbNotification> > mKeyedNotifications;
};

QT_END_HEADER

#endif // JSONDB_EPHEMERAL_PARTITION_H
