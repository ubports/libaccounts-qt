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

#include "provider.h"
#include "accountscommon.h"

#undef signals
#include <libaccounts-glib/ag-provider.h>


using namespace Accounts;

namespace Accounts {
/*!
 * @class Provider
 * @headerfile provider.h Accounts/Provider
 *
 * @brief Representation of an account provider.
 *
 * @details The Provider object represents an account provider. It can be used
 * to retrieve some basic properties of the provider (such as the name) and to
 * get access to the contents of the XML file which defines it.
 */
}; // namespace

Provider::Provider(AgProvider *provider)
    : m_provider(provider)
{
    TRACE();
    ag_provider_ref(m_provider);
}

Provider::~Provider()
{
    TRACE();

    ag_provider_unref(m_provider);
    m_provider = 0;
}

QString Provider::name() const
{
    return UTF8(ag_provider_get_name(m_provider));
}

QString Provider::displayName() const
{
    return UTF8(ag_provider_get_display_name(m_provider));
}

/*!
 * @return The name of the translation catalog, which can be used to
 * translate the displayName().
 */
QString Provider::trCatalog() const
{
    return ASCII(ag_provider_get_i18n_domain(m_provider));
}

/*!
 * @return The provider icon name.
 */
QString Provider::iconName() const
{
    return ASCII(ag_provider_get_icon_name(m_provider));
}

/*!
 * @return The DOM of the whole XML provider file.
 */
const QDomDocument Provider::domDocument() const
{
    if (doc.isNull())
    {
        const gchar *data;

        ag_provider_get_file_contents(m_provider, &data);

        QString errorStr;
        int errorLine;
        int errorColumn;
        if (!doc.setContent(QByteArray(data), true,
                            &errorStr, &errorLine, &errorColumn))
        {
            QString message(ASCII("Parse error reading account provider file "
                                  "at line %1, column %2:\n%3"));
            message.arg(errorLine).arg(errorColumn).arg(errorStr);
            qWarning() << __PRETTY_FUNCTION__ << message;
            return QDomDocument();
        }
    }

    return doc;
}

AgProvider *Provider::provider() const
{
    return m_provider;
}

