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

#ifndef JSONDB_ERRORS_H
#define JSONDB_ERRORS_H

#include "jsondb-global.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE_JSONDB

class JsonDbError {
public:
    enum ErrorCode {
        NoError = 0,
        InvalidMessage           = 1,  // Unable to parse the query message
        InvalidRequest           = 2,  // Request object doesn't contain correct elements
        MissingObject            = 3,  // Invalid or missing "object" field
        DatabaseError            = 4,  // Error directly from the database
        MissingUUID              = 5,  // Missing id field
        MissingType              = 6,  // Missing _type field
        MissingQuery             = 7,  // Missing query field
        InvalidLimit             = 8,  // Invalid limit field
        InvalidOffset            = 9,  // Invalid offset field
        MismatchedNotifyId       = 10, // Request to delete notify doesn't match existing notification
        InvalidActions           = 11, // List of actions supplied to setNotification is invalid
        UpdatingStaleVersion     = 12, // Updating stale version of object
        OperationNotPermitted    = 13,
        QuotaExceeded            = 14,
        FailedSchemaValidation   = 15, // Invalid according to the schema
        InvalidMap               = 16, // The Map definition is invalid
        InvalidReduce            = 17, // The Reduce definition is invalid
        InvalidSchemaOperation   = 18,
        InvalidPartition         = 19,
        InvalidIndexOperation    = 20
    };
};

QT_END_NAMESPACE_JSONDB

QT_END_HEADER

#endif // JSONDB_ERRORS_H