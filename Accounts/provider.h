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
/*!
 * @copyright Copyright (C) 2009-2010 Nokia Corporation.
 * @license LGPL
 */

#ifndef PROVIDER_H
#define PROVIDER_H


#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QXmlStreamReader>
#include <QDomDocument>

#include "Accounts/accountscommon.h"

extern "C"
{
    typedef struct _AgProvider AgProvider;
}

namespace Accounts
{
class Provider;

typedef QList<Provider*> ProviderList;

/*!
 * @class Provider
 *
 * @brief Representation of an account provider.
 *
 * @details The Provider object represents an account provider. It can be used
 * to retrieve some basic properties of the provider (such as the name) and to
 * get access to the contents of the XML file which defines it.
 */
class ACCOUNTS_EXPORT Provider
{
public:
    QString name() const;
    QString displayName() const;
    /*!
     * Returns the name of the translation catalog, which can be used to
     * translate the displayName().
     */
    QString trCatalog() const;

    /*!
      * Returns the provider icon name
      */
    QString iconName() const;

    /*!
     * Return the DOM of the whole XML provider file.
     */
    const QDomDocument domDocument() const;

    // \cond
    AgProvider *provider() const;

private:
    ~Provider();

    friend class Manager;
    Provider(AgProvider *provider);
    AgProvider *m_provider;
    mutable QDomDocument doc;
    // \endcond
};

} //namespace Accounts

#endif // PROVIDER_H
