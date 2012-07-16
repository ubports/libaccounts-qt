/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * This file is part of libaccounts-qt
 *
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

#include "application.h"
#include "service.h"

#undef signals
#include <libaccounts-glib/ag-application.h>

using namespace Accounts;

namespace Accounts {
/*!
 * @class Application
 * @headerfile application.h Accounts/Application
 *
 * @brief Information on the client applications of libaccounts.
 *
 * @details The Application structure holds information on the client
 * applications registered with libaccounts.
 * It is instantiated by Manager::application() and Manager::applicationList().
 */
}; // namespace

Application::Application(AgApplication *application):
    m_application(application)
{
}

/*!
 * Copy constructor. Copying an Application object is very cheap, because the
 * data is shared among copies.
 */
Application::Application(const Application &other):
    m_application(other.m_application)
{
    if (m_application != 0)
        ag_application_ref(m_application);
}

Application &Application::operator=(const Application &other)
{
    if (m_application == other.m_application) return *this;
    if (m_application != 0)
        ag_application_unref(m_application);
    m_application = other.m_application;
    if (m_application != 0)
        ag_application_ref(m_application);
    return *this;
}

/*!
 * Destructor.
 */
Application::~Application()
{
    if (m_application != 0) {
        ag_application_unref(m_application);
        m_application = 0;
    }
}

/*!
 * Check whether this object represents an Application.
 * @return true if the Application is a valid one.
 */
bool Application::isValid() const
{
    return m_application != 0;
}

/*!
 * Get the name of the application.
 * @return The application name.
 */
QString Application::name() const
{
    return UTF8(ag_application_get_name(m_application));
}

/*!
 * Get the description of the application.
 * @return The application description.
 */
QString Application::description() const
{
    return UTF8(ag_application_get_description(m_application));
}

/*!
 * Get the translation catalog for the texts returned by the methods of this
 * class.
 * @return The translation catalog name.
 */
QString Application::trCatalog() const
{
    return UTF8(ag_application_get_i18n_domain(m_application));
}

/*!
 * Get the description from the application XML file, for the specified
 * service; if not found, get the service-type description instead.
 * @return Usage description of the service.
 */
QString Application::serviceUsage(const Service &service) const
{
    return UTF8(ag_application_get_service_usage(m_application,
                                                 service.service()));
}
