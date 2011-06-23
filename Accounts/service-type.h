/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * This file is part of libaccounts-qt
 *
 * Copyright (C) 2011 Nokia Corporation.
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
/*!
 * @copyright Copyright (C) 2009-2011 Nokia Corporation.
 * @license LGPL
 */

#ifndef ACCOUNTS_SERVICE_TYPE_H
#define ACCOUNTS_SERVICE_TYPE_H


#include <QObject>
#include <QDomDocument>

#include "Accounts/accountscommon.h"

extern "C"
{
    typedef struct _AgServiceType AgServiceType;
}

namespace Accounts
{

/*!
 * @class ServiceType
 *
 * @brief Representation of an account service type.
 *
 * @details The ServiceType object represents an account service type. It can
 * be used to retrieve some basic properties of the service type (such as
 * name and icon) and to get access to the contents of the XML file which
 * defines it.
 */
class ACCOUNTS_EXPORT ServiceType
{
public:
    /*!
     * Returns the name (ID) of the service type.
     */
    QString name() const;

    /*!
     * @return The display name of the service type; this is a string that
     * could be shown in the UI to describe the service type to the user.
     *
     * The library attempts to translate this string by passing it to the
     * qtTrId() function; in order for this to work you must make sure that
     * the translation catalogue has been loaded before, if needed.
     */
    QString displayName() const;

    /*!
     * @return The name of the translation catalog, which can be used to
     * translate the displayName()
     */
    QString trCatalog() const;

    /*!
     * @return The icon name
     */
    QString iconName() const;

    /*!
     * @return The DOM of the whole XML service file
     */
    const QDomDocument domDocument() const;

private:
    // \cond
    ~ServiceType();

    friend class Manager;
    ServiceType(AgServiceType *serviceType);
    AgServiceType *m_serviceType;
    mutable QDomDocument doc;
    // \endcond
};

} //namespace Accounts

#endif // ACCOUNTS_SERVICE_TYPE_H
