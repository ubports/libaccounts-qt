/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * This file is part of libaccounts-qt
 *
 * Copyright (C) 2009-2011 Nokia Corporation.
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

#include "service-type.h"

#include <libaccounts-glib/ag-manager.h>
#include <libaccounts-glib/ag-service-type.h>
#include "manager.h"
#include <QtDebug>
#include <QtGlobal>


using namespace Accounts;

ServiceType::ServiceType(AgServiceType *serviceType)
    : m_serviceType(serviceType)
{
    TRACE();
    ag_service_type_ref(m_serviceType);
}

ServiceType::~ServiceType()
{
    TRACE();
    ag_service_type_unref(m_serviceType);
    m_serviceType = 0;
}

QString ServiceType::name() const
{
    return UTF8(ag_service_type_get_name(m_serviceType));
}

QString ServiceType::displayName() const
{
    const gchar *id;

    /* libaccounts-glib returns the display name untranslated. */
    id = ag_service_type_get_display_name(m_serviceType);
    if (id != NULL) {
        return qtTrId(id);
    } else {
        return QString();
    }
}

QString ServiceType::trCatalog() const
{
    return ASCII(ag_service_type_get_i18n_domain(m_serviceType));
}

QString ServiceType::iconName() const
{
    return ASCII(ag_service_type_get_icon_name(m_serviceType));
}

const QDomDocument ServiceType::domDocument() const
{
    if (doc.isNull()) {
        const gchar *data;
        gsize len;

        ag_service_type_get_file_contents(m_serviceType, &data, &len);

        QString errorStr;
        int errorLine;
        int errorColumn;
        if (!doc.setContent(QByteArray(data, len), true,
                            &errorStr, &errorLine, &errorColumn)) {
            QString message(ASCII("Parse error reading serviceType file "
                                  "at line %1, column %2:\n%3"));
            message.arg(errorLine).arg(errorColumn).arg(errorStr);
            qWarning() << __PRETTY_FUNCTION__ << message;
            return QDomDocument();
        }
    }

    return doc;
}

