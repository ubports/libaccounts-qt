/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
* This file is part of libaccounts-qt
*
* Copyright (C) 2012 Nokia Corporation.
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

#ifndef ACCOUNTS_ACCOUNT_SERVICE_H
#define ACCOUNTS_ACCOUNT_SERVICE_H

#include <QObject>
#include <QStringList>

#include "Accounts/account.h"

namespace Accounts
{
class Manager;
class Service;

class AccountServicePrivate;
class ACCOUNTS_EXPORT AccountService: public QObject
{
    Q_OBJECT

public:
    explicit AccountService(Account *account, Service *service);
    virtual ~AccountService();

    Account *account() const;
    Service *service() const;

    bool enabled() const;

    QStringList allKeys() const;

    void beginGroup(const QString &prefix);

    QStringList childGroups() const;
    QStringList childKeys() const;

    void clear();

    bool contains(const QString &key) const;

    void endGroup();

    QString group() const;

    void remove(const QString &key);

    void setValue(const char *key, const QVariant &value);
    void setValue(const QString &key, const QVariant &value);

    QVariant value(const QString &key, SettingSource *source = 0) const;
    QVariant value(const char *key, SettingSource *source = 0) const;

    QStringList changedFields() const;

signals:
    void enabled(bool isEnabled);
    void changed();

private:
    // Don't include private data in docs: \cond
    AccountServicePrivate *d_ptr;
    Q_DECLARE_PRIVATE(AccountService)
    // \endcond
};

typedef QList<AccountService*> AccountServiceList;

} //namespace

#endif // ACCOUNTS_ACCOUNT_SERVICE_H
