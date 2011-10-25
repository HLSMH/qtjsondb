/****************************************************************************
**
** Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsonconversion.h"
#include "qjsengine.h"

#include <QtJsonDbQson/private/qson_p.h>

namespace QtAddOn { namespace JsonDb {

static QJSValue qsonListToJSValue(const QsonList &list, QJSEngine *engine)
{
    int count = list.count();
    QJSValue result = engine->newArray(count);

    for (int i = 0; i < count; ++i)
        result.setProperty(i, qsonToJSValue(list.at<QsonElement>(i), engine));
    return result;
}

static QJSValue qsonMapToJSValue(const QsonMap &map, QJSEngine *engine)
{
    QJSValue result = engine->newObject();
    QStringList keys = map.keys();
    foreach (const QString &key, keys)
        result.setProperty(key, qsonToJSValue(map.value<QsonElement>(key), engine));
    return result;
}

QJSValue qsonToJSValue(const QsonObject &object, QJSEngine *engine)
{
    switch (object.type()) {
    case QsonObject::ListType:
        return qsonListToJSValue(object.toList(), engine);
    case QsonObject::MapType:
        return qsonMapToJSValue(object.toMap(), engine);
    case QsonObject::StringType:
        return QJSValue(engine, QsonElement(object).value<QString>());
    case QsonObject::DoubleType:
        return QJSValue(engine, QsonElement(object).value<double>());
    case QsonObject::IntType:
        return QJSValue(engine, QsonElement(object).value<int>());
    case QsonObject::UIntType:
        return QJSValue(engine, QsonElement(object).value<uint>());
    case QsonObject::BoolType:
        return QJSValue(engine, QsonElement(object).value<bool>());
    case QsonObject::NullType:
        return engine->nullValue();
    default:
        break;
    }
    return QJSValue();
}

QsonObject jsValueToQson(const QJSValue &object)
{
    return variantToQson(object.toVariant());
}

} } // end namespace QtAddOn::JsonDb
