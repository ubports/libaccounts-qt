/*
 * This file is part of libaccounts-qt
 *
 * Copyright (C) 2010 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "error.h"

#include <libaccounts-glib/ag-errors.h>

namespace Accounts {

Error::Error(const GError *error)
{
    registerType();

    if (error == NULL) {
        m_type = NoError;
        m_message = QString();
    } else {
        if (error->domain == AG_ERRORS) {
            switch (error->code) {
            case AG_ERROR_DB:
                m_type = Database;
                break;
            case AG_ERROR_DELETED:
                m_type = Deleted;
                break;
            case AG_ERROR_DISPOSED:
                // Should never happen here!
                qCritical() << Q_FUNC_INFO << "Account object is disposed!";
                m_type = Unknown;
                break;
            case AG_ERROR_DB_LOCKED:
                m_type = DatabaseLocked;
                break;
            case AG_ERROR_ACCOUNT_NOT_FOUND:
                m_type = AccountNotFound;
                break;
            default:
                qWarning() << Q_FUNC_INFO << "Unknown error:" << error->code;
                m_type = Unknown;
                break;
            }
        } else {
            // The error is coming from another domain; this shouldn't happen
            qCritical() << Q_FUNC_INFO << "Error is coming from unknown domain";
            m_type = Unknown;
        }

        m_message = QString::fromUtf8(error->message);
    }
}

}; // namespace

