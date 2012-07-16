/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * This file is part of libaccounts-qt
 *
 * Copyright (C) 2009-2011 Nokia Corporation.
 * Copyright (C) 2012 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#undef signals
#include <libaccounts-glib/ag-service-type.h>

using namespace Accounts;

namespace Accounts {
/*!
 * @class ServiceType
 * @headerfile service-type.h Accounts/ServiceType
 *
 * @brief Representation of an account service type.
 *
 * @details The ServiceType object represents an account service type. It can
 * be used to retrieve some basic properties of the service type (such as
 * name and icon) and to get access to the contents of the XML file which
 * defines it.
 */
}; // namespace

ServiceType::ServiceType(AgServiceType *serviceType, ReferenceMode mode):
    m_serviceType(serviceType)
{
    TRACE();
    if (m_serviceType != 0 && mode == AddReference)
        ag_service_type_ref(m_serviceType);
}

/*!
 * Construct an invalid serviceType.
 */
ServiceType::ServiceType():
    m_serviceType(0)
{
}

/*!
 * Copy constructor. Copying a ServiceType object is very cheap, because the
 * data is shared among copies.
 */
ServiceType::ServiceType(const ServiceType &other):
    m_serviceType(other.m_serviceType)
{
    if (m_serviceType != 0)
        ag_service_type_ref(m_serviceType);
}

ServiceType &ServiceType::operator=(const ServiceType &other)
{
    if (m_serviceType == other.m_serviceType) return *this;
    if (m_serviceType != 0)
        ag_service_type_unref(m_serviceType);
    m_serviceType = other.m_serviceType;
    if (m_serviceType != 0)
        ag_service_type_ref(m_serviceType);
    return *this;
}

ServiceType::~ServiceType()
{
    TRACE();
    if (m_serviceType != 0) {
        ag_service_type_unref(m_serviceType);
        m_serviceType = 0;
    }
}

/*!
 * Check whether this object represents a ServiceType.
 * @return true if the ServiceType is a valid one.
 */
bool ServiceType::isValid() const
{
    return m_serviceType != 0;
}

/*!
 * Returns the name (ID) of the service type.
 */
QString ServiceType::name() const
{
    return UTF8(ag_service_type_get_name(m_serviceType));
}

/*!
 * @return The display name of the service type; this is a string that
 * could be shown in the UI to describe the service type to the user.
 *
 * The library attempts to translate this string by passing it to the
 * qtTrId() function; in order for this to work you must make sure that
 * the translation catalogue has been loaded before, if needed.
 */
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

/*!
 * @return The name of the translation catalog, which can be used to
 * translate the displayName()
 */
QString ServiceType::trCatalog() const
{
    return ASCII(ag_service_type_get_i18n_domain(m_serviceType));
}

/*!
 * @return The icon name
 */
QString ServiceType::iconName() const
{
    return ASCII(ag_service_type_get_icon_name(m_serviceType));
}

/*!
 * @return The DOM of the whole XML service file
 */
const QDomDocument ServiceType::domDocument() const
{
    const gchar *data;
    gsize len;

    ag_service_type_get_file_contents(m_serviceType, &data, &len);

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(QByteArray(data, len), true,
                        &errorStr, &errorLine, &errorColumn)) {
        QString message(ASCII("Parse error reading serviceType file "
                              "at line %1, column %2:\n%3"));
        message.arg(errorLine).arg(errorColumn).arg(errorStr);
        qWarning() << __PRETTY_FUNCTION__ << message;
    }

    return doc;
}

