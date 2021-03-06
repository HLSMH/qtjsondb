/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtAddOn.JsonDb module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QJSONDB_CONNECTION_H
#define QJSONDB_CONNECTION_H

#include <QtCore/QObject>

#include <QtJsonDb/qjsondbglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_JSONDB

class QJsonDbRequest;
class QJsonDbWatcher;

class QJsonDbConnectionPrivate;
class Q_JSONDB_EXPORT QJsonDbConnection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString socketName READ socketName WRITE setSocketName)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool autoReconnectEnabled READ isAutoReconnectEnabled WRITE setAutoReconnectEnabled)

public:
    QJsonDbConnection(QObject *parent = 0);
    ~QJsonDbConnection();

    void setSocketName(const QString &);
    QString socketName() const;

    enum ErrorCode {
        NoError = 0
    };

    enum Status {
        Unconnected = 0,
        Connecting  = 1,
        Authenticating = 2,
        Connected = 3
    };
    Status status() const;

    void setAutoReconnectEnabled(bool value);
    bool isAutoReconnectEnabled() const;

    QList<QJsonDbRequest *> pendingRequests() const;

    static void setDefaultConnection(QJsonDbConnection *);
    static QJsonDbConnection *defaultConnection();

public Q_SLOTS:
    void connectToServer();
    void disconnectFromServer();

    bool send(QJsonDbRequest *request);
    bool cancel(QJsonDbRequest *request);
    void cancelPendingRequests();

    bool addWatcher(QJsonDbWatcher *watcher);
    bool removeWatcher(QJsonDbWatcher *watcher);

Q_SIGNALS:
    void connected();
    void disconnected();
    void error(QtJsonDb::QJsonDbConnection::ErrorCode error, const QString &message);

    // signals for properties
    void statusChanged(QtJsonDb::QJsonDbConnection::Status newStatus);

private:
    Q_DISABLE_COPY(QJsonDbConnection)
    Q_DECLARE_PRIVATE(QJsonDbConnection)
    QScopedPointer<QJsonDbConnectionPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_onDisconnected())
    Q_PRIVATE_SLOT(d_func(), void _q_onConnected())
    Q_PRIVATE_SLOT(d_func(), void _q_onError(QLocalSocket::LocalSocketError))
    Q_PRIVATE_SLOT(d_func(), void _q_onTimer())
    Q_PRIVATE_SLOT(d_func(), void _q_onReceivedObject(QJsonObject))
    Q_PRIVATE_SLOT(d_func(), void _q_onAuthFinished())

    Q_PRIVATE_SLOT(d_func(), void _q_privateReadRequestStarted(int,quint32,QString))
    Q_PRIVATE_SLOT(d_func(), void _q_privateWriteRequestStarted(int,quint32))
    Q_PRIVATE_SLOT(d_func(), void _q_privateFlushRequestStarted(int,quint32))
    Q_PRIVATE_SLOT(d_func(), void _q_privateRequestFinished(int))
    Q_PRIVATE_SLOT(d_func(), void _q_privateRequestError(int,QtJsonDb::QJsonDbRequest::ErrorCode,QString))
    Q_PRIVATE_SLOT(d_func(), void _q_privateRequestResultsAvailable(int,QList<QJsonObject>))

    friend class QJsonDbWatcher;
};

QT_END_NAMESPACE_JSONDB

QT_END_HEADER

#endif // QJSONDB_CONNECTION_H
