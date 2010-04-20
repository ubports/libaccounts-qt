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
#ifndef ACCOUNTSTEST_H
#define ACCOUNTSTEST_H

#include <QObject>

#include "Accounts/Account"

using namespace Accounts;

class AccountsTest : public QObject
{
    Q_OBJECT

public:

public slots:
    void stored();
    void error(Accounts::ErrorCode errorCode);
    void enabled(const QString &serviceName, bool enabled);
    void enabledEvent(Accounts::AccountId id);
    void display(const QString &serviceName);
    void created(Accounts::AccountId id);
    void removed();
    void w_server_notify(const char *);
    void w_port_notify(const char *);
    void w_parameters_notify(const char *);
private slots:
    void initTestCase();
    void cleanupTestCase();

    void managerTestCase();
    void createAccountTestCase();
    void accountTestCase();
    void accountListTestCase();

    void providerTestCase();
    void serviceTestCase();
    void serviceListTestCase();

    void serviceConstTestCase();

    void accountConstTestCase();

    void accountServiceTestCase();
    void accountEnabledTestCase();
    void accountDisplayNameTestCase();

    void accountValueTestCase();

    void accountSyncTestCase();

    void createdTestCase();

    void removeTestCase();

    void watchesTest();

    void serviceDataTest();

    void settingsTest();

    void keySignVerifyTest();

    void incrementalAccountIdsTest();

    void selectGlobalAccountSettingsTest();

    void credentialsIdTest();

    void listEnabledServices();

    void listEnabledByServiceType();

    void enabledEvent();

    private:
    bool m_stored;
    bool m_enabled;
    AccountId m_created;
    AccountId m_enabledEvent;
    bool m_removed;
    int m_rowsInserted;
    int m_rowsRemoved;
    int m_server_notify;
    int m_port_notify;
    int m_parameters_notify;
};

#endif
