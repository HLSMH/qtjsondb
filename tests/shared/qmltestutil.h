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

#ifndef QMLTESTUTIL_H
#define QMLTESTUTIL_H

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlExpression>
#include <QQmlError>
#include <QQmlProperty>
#include <QDir>

inline QVariant createObject(const QString &functionName)
{
    static QStringList greekAlphabets;
    static int size = 0;
    if (greekAlphabets.isEmpty()) {
        greekAlphabets  << "alpha" << "beta" << "gamma" << "epsilon" << "zeta"
                        << "eta" << "theta"<< "iota" << "kappa"  << "lambda";
        size = greekAlphabets.size();
    }

    QVariantMap obj;
    obj.insert("_type", functionName);
    int position = qrand()%size;
    obj.insert("alphabet", greekAlphabets[position]);
    obj.insert("pos", position);
    return obj;
}

inline QVariant createObjectList(const QString &functionName, int size)
{
    QVariantList list;
    for (int i = 0; i<size; i++) {
        list.append(createObject(functionName));
    }
    return list;
}

inline QVariant updateObjectList(const QVariantList &objects, const QVariantList &extra)
{
    QVariantList list;
    for (int i = 0; i<objects.size(); i++) {
        QVariantMap objMap = objects[i].toMap();
        QVariantMap extraMap = extra[i].toMap();
        QVariantMap::Iterator j = extraMap.begin();
        while (j != extraMap.end()) {
            objMap.insert(j.key(), j.value());
            ++j;
        }
        list.append(objMap);
    }
    return list;
}

inline QVariant updateObject(const QVariant &object, const QVariantList &extra)
{
    QVariantMap objMap = object.toMap();
    QVariantMap extraMap = extra[0].toMap();
    QVariantMap::Iterator j = extraMap.begin();
    while (j != extraMap.end()) {
        objMap.insert(j.key(), j.value());
        ++j;
    }
    return objMap;
}


inline QString objectString(const QString &key, const QVariant &value)
{
    QString fullObject;
    if (!key.isEmpty()) {
        fullObject = QString("%1:").arg(key);
    }
    if (value.type() == QVariant::Map) {
        fullObject += "{";
        QVariantMap map = value.toMap();
        QVariantMap::Iterator i = map.begin();
        int count = 0;
        while (i != map.end()) {
            if (count)
                fullObject += ",";
            fullObject += objectString(i.key(),i.value());
            ++i;
            ++count;
        }
        fullObject += "}";
    } else if (value.type() == QVariant::List) {
        fullObject += "[";
        QVariantList list = value.toList();
        QVariantList::Iterator i = list.begin();
        int count = 0;
        while (i != list.end()) {
            if (count)
                fullObject += ",";
            fullObject += objectString(QString(),*i);
            ++i;
            ++count;
        }
        fullObject += "]";

    } else {
        fullObject += QString("'%1'").arg(value.toString());
    }
    return fullObject;
}

inline QString findQMLPluginPath(const QString &pluginName)
{
    QString pluginPath;
    qsrand(QTime::currentTime().msec());
    QQmlEngine *engine = new QQmlEngine();
    QStringList pluginPaths = engine->importPathList();
    for (int i=0; (i<pluginPaths.count() && pluginPath.isEmpty()); i++) {
        QDir dir(pluginPaths[i]+"/"+pluginName);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QList<QQmlError> errors;
            if (engine->importPlugin(list.at(i).absoluteFilePath(), pluginName, &errors)) {
                pluginPath = list.at(i).absoluteFilePath();
                break;
            }
        }
    }
    delete engine;
    return pluginPath;
}

class ComponentData {
public:
    ComponentData() :engine(0), component(0), qmlElement(0) {}
    ~ComponentData()
    {
        delete qmlElement;
        delete component;
        delete engine;
    }
    QQmlEngine *engine;
    QQmlComponent *component;
    QObject *qmlElement;
};

#endif // QMLTESTUTIL_H
