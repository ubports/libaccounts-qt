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

class ACCOUNTS_EXPORT ServiceType
{
public:
    QString name() const;
    QString displayName() const;
    QString trCatalog() const;
    QString iconName() const;
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
