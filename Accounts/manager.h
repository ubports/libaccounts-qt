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

#ifndef ACCOUNTS_MANAGER_H
#define ACCOUNTS_MANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>

#include "Accounts/accountscommon.h"
#include "Accounts/account.h"
#include "Accounts/error.h"
#include "Accounts/provider.h"
#include "Accounts/service.h"
#include "Accounts/service-type.h"

/*!
 * @namespace Accounts
 */
namespace Accounts
{

class AccountService;
class Application;

typedef QList<Application> ApplicationList;

class ACCOUNTS_EXPORT Manager: public QObject
{
    Q_OBJECT

public:
    Manager(QObject *parent = 0);
    Manager(const QString &serviceType, QObject *parent = 0);
    ~Manager();

    Account *account(const AccountId &id) const;

    AccountIdList accountList(const QString &serviceType = QString::null) const;
    AccountIdList accountListEnabled(const QString &serviceType = QString::null) const;

    Account *createAccount(const QString &providerName);

    Service service(const QString &serviceName) const;
    ServiceList serviceList(const QString &serviceType = QString::null) const;

    Provider provider(const QString &providerName) const;
    ProviderList providerList() const;

    ServiceType serviceType(const QString &name) const;

    Application application(const QString &applicationName) const;
    ApplicationList applicationList(const Service &service) const;

    QString serviceType() const;

    void setTimeout(quint32 timeout);
    quint32 timeout();

    void setAbortOnTimeout(bool abort);
    bool abortOnTimeout() const;

    Error lastError() const;

Q_SIGNALS:
    void accountCreated(Accounts::AccountId id);
    void accountRemoved(Accounts::AccountId id);
    void accountUpdated(Accounts::AccountId id);
    void enabledEvent(Accounts::AccountId id);

private:

    // \cond
    class Private;
    friend class Private;
    Private *d; // Owned.

    friend class Account;
    friend class AccountService;
    // \endcond
}; // Manager

} //namespace Accounts

#endif // ACCOUNTS_MANAGER_H
