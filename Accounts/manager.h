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

#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>

#include "Accounts/accountscommon.h"
#include "Accounts/account.h"
#include "Accounts/provider.h"
#include "Accounts/service.h"

/*! @namespace Accounts
 *
 */
namespace Accounts
{

/*!
 * @class Manager
 *
 * @brief Manager of accounts, services and providers.
 *
 * @details The Manager offers ways to create accounts, list accounts, services
 * and providers. It also emits signals when accounts are created and removed.
 */
class Manager : public QObject
{
    Q_OBJECT

public:

    /*!
     * Constructor
     */
    Manager(QObject *parent = 0);

    ~Manager();

    /*!
     * Loads an accont from the database.
     * @param id Id of the account to be retrieved.
     *
     * @return Requested account or NULL if not found.
     */
    Account *account(const AccountId &id) const;

    /*!
     * Lists the accounts which support the requested service.
     *
     * @param serviceType Type of service that returned accounts must support.
     * If not given, all accounts are returned.
     *
     * @return List of account IDs.
     */
    AccountIdList accountList(const QString &serviceType = 0) const;

    /*!
     * Creates a new account.
     * @param providerName Name of account provider.
     *
     * @return Created account or NULL if some error occurs.
     */
    Account *createAccount(const QString &providerName);

    /*!
     * Get an object representing a service.
     * @param serviceName Name of service to get.
     *
     * @return Requested service or NULL if not found.
     */
    Service *service(const QString &serviceName) const;

    /*!
     * Get service list.
     * @param serviceType Type of services to be listed. If not given, all
     * services are listed.
     *
     * @return List of Service objects.
     */
    ServiceList serviceList(const QString &serviceType = 0) const;

    /*!
     * Get an object representing a provider.
     * @param providerName Name of provider to get.
     *
     * @return Requested provider or NULL if not found.
     */
    Provider *provider(const QString &providerName) const;

    /*!
     * Get provider list.
     *
     * @return List of registered providers.
     */
    ProviderList providerList() const;


signals:

    void accountCreated(Accounts::AccountId id);
    void accountRemoved(Accounts::AccountId id);

private:

    // \cond
    class Private;
    friend class Private;
    Private *d; // Owned.

    Provider *providerInstance(AgProvider *provider) const;
    Service *serviceInstance(AgService *service) const;
    friend class Account;
    // \endcond
}; // Manager

} //namespace Accounts

#endif // ACCOUNTMANAGER_H
