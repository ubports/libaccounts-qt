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
/*!
 * @copyright Copyright (C) 2009-2011 Nokia Corporation.
 * @license LGPL
 */

#ifndef SERVICE_H
#define SERVICE_H


#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QXmlStreamReader>
#include <QDomDocument>

#include "Accounts/accountscommon.h"

extern "C"
{
    typedef struct _AgService AgService;
}

namespace Accounts
{
class Service;

typedef QList<Service*> ServiceList;

class ACCOUNTS_EXPORT Service
{
public:
    QString name() const;
    QString displayName() const;
    QString trCatalog() const;
    QString serviceType() const;
    QString provider() const;
    QString iconName() const;

    /*!
     * Creates a QXmlStreamReader for the service XML file, positioned at the
     * "type_data" element.
     *
     * @deprecated This method is deprecated and will eventually be removed.
     */
    QXmlStreamReader *xmlStreamReader() const;

    const QDomDocument domDocument() const;

    // \cond
    AgService *service() const;

private:
    ~Service();

    friend class Account;
    friend class Manager;
    Service(AgService *service);
    AgService *m_service;
    mutable QDomDocument doc;
    // \endcond
};

} //namespace Accounts

#endif // SERVICE_H
