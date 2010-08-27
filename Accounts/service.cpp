/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * This file is part of libaccounts-qt
 *
 * Copyright (C) 2009-2010 Nokia Corporation.
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

#include "account.h"
#include "manager.h"

#include <libaccounts-glib/ag-manager.h>
#include <libaccounts-glib/ag-account.h>
#include <libaccounts-glib/ag-service.h>
#include <QtDebug>


using namespace Accounts;

Service::Service(AgService *service)
    : m_service(service)
{
    TRACE();
    ag_service_ref(m_service);
}

Service::~Service()
{
    TRACE();

    ag_service_unref(m_service);
    m_service = 0;
}

QString Service::name() const
{
    return UTF8(ag_service_get_name(m_service));
}

QString Service::displayName() const
{
    return UTF8(ag_service_get_display_name(m_service));
}

QString Service::serviceType() const
{
    return ASCII(ag_service_get_service_type(m_service));
}

QString Service::trCatalog() const
{
    return ASCII(ag_service_get_i18n_domain(m_service));
}

QString Service::provider() const
{
    return UTF8(ag_service_get_provider(m_service));
}

QString Service::iconName() const
{
    return ASCII(ag_service_get_icon_name(m_service));
}

QXmlStreamReader *Service::xmlStreamReader() const
{
    const gchar *data;
    gsize offset;

    ag_service_get_file_contents(m_service, &data, &offset);
    if (data)
        data += offset;

    QXmlStreamReader *reader = new QXmlStreamReader(QByteArray(data));

    /* Read the startDocument token */
    if (reader->readNext() != QXmlStreamReader::StartDocument)
    {
        delete reader;
        return NULL;
    }

    return reader;
}

const QDomDocument Service::domDocument() const
{
    if (doc.isNull())
    {
        const gchar *data;

        ag_service_get_file_contents(m_service, &data, NULL);

        QString errorStr;
        int errorLine;
        int errorColumn;
        if (!doc.setContent(QByteArray(data), true,
                            &errorStr, &errorLine, &errorColumn))
        {
            QString message(ASCII("Parse error reading account service file "
                                  "at line %1, column %2:\n%3"));
            message.arg(errorLine).arg(errorColumn).arg(errorStr);
            qWarning() << __PRETTY_FUNCTION__ << message;
            return QDomDocument();
        }
    }

    return doc;
}

AgService *Service::service() const
{
    return m_service;
}

