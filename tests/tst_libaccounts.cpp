/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * This file is part of libaccounts-qt
 *
 * Copyright (C) 2009-2011 Nokia Corporation.
 * Copyright (C) 2015-2016 Canonical Ltd.
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
#include <QtTest/QtTest>
#include <QSignalSpy>

#include "Accounts/Account"
#include "Accounts/Application"
#include "Accounts/Manager"
#include "Accounts/AccountService"

using namespace Accounts;

#define PROVIDER QStringLiteral("dummyprovider")
#define SERVICE QStringLiteral("dummyservice")
#define MYSERVICE QStringLiteral("MyService")
#define OTHERSERVICE QStringLiteral("OtherService")
#define EMAIL_SERVICE_TYPE QStringLiteral("e-mail")

Q_DECLARE_METATYPE(Accounts::AccountId)
Q_DECLARE_METATYPE(const char *)

class AccountsTest: public QObject
{
    Q_OBJECT

public:
    AccountsTest() {
        qRegisterMetaType<AccountId>("Accounts::AccountId");
        qRegisterMetaType<const char *>("const char *");
    }

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testManager();
    void testCreateAccount();
    void testAccount();
    void testObjectsLifetime();
    void testAccountList();

    void testProvider();
    void testService();
    void testServiceList();
    void testServiceConst();
    void testAccountConst();

    void testAccountProvider();
    void testAccountServices();
    void testAccountEnabled();
    void testAccountDisplayName();
    void testAccountValue();
    void testAccountSync();

    void testCreated();
    void testRemove();

    void testAccountService();

    void testWatches();

    void testServiceData();
    void testSettings();

    void testKeySignVerify();

    void testIncrementalAccountIds();

    void testSelectGlobalAccountSettings();

    void testCredentialsId();
    void testAuthData();
    void testGlobalAuthData();

    void testListEnabledServices();
    void testListEnabledByServiceType();
    void testEnabledEvent();
    void testServiceType();
    void testUpdateAccount();
    void testApplication();
    void testApplicationListServices();

public Q_SLOTS:
    void onAccountServiceChanged();

private:
    QStringList m_accountServiceChangedFields;
};

void clearDb()
{
    QDir dbroot(QString(getenv("ACCOUNTS")));
    dbroot.remove(QString("accounts.db"));
}

void AccountsTest::initTestCase()
{
    //init
    setenv("ACCOUNTS", "/tmp/", false);
    setenv("AG_APPLICATIONS", DATA_PATH, false);
    setenv("AG_SERVICES", DATA_PATH, false);
    setenv("AG_SERVICE_TYPES", DATA_PATH, false);
    setenv("AG_PROVIDERS", DATA_PATH, false);
    setenv("XDG_DATA_HOME", DATA_PATH, false);
    //clear database
    clearDb();
}

void AccountsTest::cleanupTestCase()
{
}

void AccountsTest::testManager()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);
    QCOMPARE(mgr->serviceType(), QString());

    mgr->setAbortOnTimeout(true);
    QCOMPARE(mgr->abortOnTimeout(), true);

    mgr->setTimeout(123);
    QCOMPARE(mgr->timeout(), quint32(123));

    QCOMPARE(mgr->options().testFlag(Manager::DisableNotifications), false);

    delete mgr;

    mgr = new Manager(Manager::DisableNotifications);
    QCOMPARE(mgr->options().testFlag(Manager::DisableNotifications), true);
    delete mgr;
}

void AccountsTest::testCreateAccount()
{
    clearDb();

    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);
    QVERIFY(account->manager() == mgr);
    delete account;

    account = mgr->createAccount(PROVIDER);
    QVERIFY(account != 0);
    delete account;

    delete mgr;
}

void AccountsTest::testAccount()
{
    clearDb();

    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    //this should not exist
    Account *account2 = mgr->account(1);
    QVERIFY(!account2);
    Error error = mgr->lastError();
    QCOMPARE(error.type(), Error::AccountNotFound);

    // make sure there is account
    Account *account = mgr->createAccount(NULL);
    account->sync();

    //now we get account
    account2 = mgr->account(1);
    QVERIFY(account2 != 0);

    delete account2;
    delete account;
    delete mgr;
}

void AccountsTest::testObjectsLifetime()
{
    clearDb();

    Manager *manager = new Manager();

    // this should not exist
    Account *account = Account::fromId(manager, 1);
    QVERIFY(account == 0);
    Error error = manager->lastError();
    QCOMPARE(error.type(), Error::AccountNotFound);

    // create an account
    account = new Account(manager, PROVIDER);
    account->setDisplayName("hi!");
    account->syncAndBlock();
    AccountId accountId = account->id();
    delete account;

    // Now load it, in two different ways
    QPointer<Account> sharedAccount = manager->account(accountId);
    QVERIFY(sharedAccount != 0);
    QCOMPARE(sharedAccount->displayName(), QString("hi!"));

    QPointer<Account> ownedAccount = Account::fromId(manager, accountId);
    QVERIFY(ownedAccount != 0);
    QCOMPARE(ownedAccount->displayName(), QString("hi!"));

    /* Load it once more from the shared interface, and see that we get the
     * same object */
    QCOMPARE(manager->account(accountId), sharedAccount.data());

    /* Delete the manager; the owned account should survive, the shared
     * shouldn't */
    delete manager;
    QVERIFY(sharedAccount == 0);
    QVERIFY(ownedAccount != 0);
    QVERIFY(ownedAccount->manager() == 0);

    delete ownedAccount;
}

void AccountsTest::testAccountList()
{
    clearDb();

    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    // make sure there is account
    Account *account = mgr->createAccount(NULL);
    account->sync();

    AccountIdList list = mgr->accountList(NULL);
    QVERIFY(!list.isEmpty());
    QCOMPARE(list.size(), 1);

    list = mgr->accountList("e-mail");
    QVERIFY(list.isEmpty());

    delete account;
    delete mgr;
}

void AccountsTest::testProvider()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account != 0);

    QString provider_name = account->providerName();
    QCOMPARE(provider_name, PROVIDER);

    delete account;

    /* Test invalid provider */
    Provider invalid;
    QVERIFY(!invalid.isValid());

    /* Check listing and loading of XML files */
    Provider provider = mgr->provider("MyProvider");
    QVERIFY(provider.isValid());

    QCOMPARE(provider.displayName(), QString("My Provider"));
    QCOMPARE(provider.iconName(), QString("general_myprovider"));
    QCOMPARE(provider.description(), QString("fast & furious"));
    QCOMPARE(provider.trCatalog(), QString("accounts"));
    QCOMPARE(provider.pluginName(), QString("generic-oauth"));
    QCOMPARE(provider.domainsRegExp(), QString(".*example.net"));
    QCOMPARE(provider.isSingleAccount(), true);

    QDomDocument dom = provider.domDocument();
    QDomElement root = dom.documentElement();
    QCOMPARE(root.tagName(), QString("provider"));

    /* Test copy constructor */
    Provider copy(provider);
    QCOMPARE(copy.displayName(), QString("My Provider"));

    /* Test assignment */
    copy = provider;
    QCOMPARE(copy.displayName(), QString("My Provider"));
    copy = invalid;
    QVERIFY(!copy.isValid());
    copy = provider;
    QCOMPARE(copy.displayName(), QString("My Provider"));

    ProviderList providers = mgr->providerList();
    QVERIFY(!providers.isEmpty());
    QVERIFY(providers.size() == 1);
    QCOMPARE(providers[0].displayName(), QString("My Provider"));

    delete mgr;
}

void AccountsTest::testService()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());
    QCOMPARE(service.name(), MYSERVICE);
    QCOMPARE(service.description(), QStringLiteral("Test description"));
    QCOMPARE(service.iconName(), QString("general_myservice"));
    QCOMPARE(service.trCatalog(), QString("accounts"));
    QStringList tags;
    tags << "email" << "e-mail";
    QCOMPARE(service.tags(), tags.toSet());
    // Called twice, because the second time it returns a cached result
    QCOMPARE(service.tags(), tags.toSet());
    QVERIFY(service.hasTag("email"));
    QVERIFY(!service.hasTag("chat"));

    service = mgr->service(SERVICE);
    QVERIFY(!service.isValid());

    delete mgr;
}

void AccountsTest::testServiceList()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    ServiceList list = mgr->serviceList();
    QVERIFY(!list.isEmpty());
    QCOMPARE(list.count(), 2);

    list = mgr->serviceList("e-mail");
    QCOMPARE(list.count(), 1);

    list = mgr->serviceList("sharing");
    QCOMPARE(list.count(), 1);

    delete mgr;
}

void AccountsTest::testServiceConst()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());
    QCOMPARE(service.name(), MYSERVICE);
    QCOMPARE(service.displayName(), QString("My Service"));
    QCOMPARE(service.serviceType(), QString("e-mail"));
    QCOMPARE(service.provider(), QString("MyProvider"));
    QVERIFY(service.hasTag(QString("email")));

    delete mgr;
}


/* account */


void AccountsTest::testAccountConst()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account != 0);
    QVERIFY(account->isWritable());

    delete account;
    delete mgr;
}

void AccountsTest::testAccountProvider()
{
    Manager *manager = new Manager();
    QVERIFY(manager != 0);

    Account *account = manager->createAccount("MyProvider");
    QVERIFY(account != 0);

    QCOMPARE(account->providerName(), QString("MyProvider"));

    Provider provider = account->provider();
    QCOMPARE(provider.name(), QString("MyProvider"));

    delete account;
    delete manager;
}

void AccountsTest::testAccountServices()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != 0);

    QVERIFY(!account->supportsService(QString("unsupported")));
    QVERIFY(account->supportsService(QString("e-mail")));

    ServiceList list = account->services();
    QVERIFY(!list.isEmpty());
    QCOMPARE(list.count(), 1);

    list = account->services("e-mail");
    QCOMPARE(list.count(), 1);

    list = account->services("unsupported");
    QVERIFY(list.isEmpty());

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    /* Test default settings */
    account->selectService(service);
    QCOMPARE(account->value("parameters/server").toString(),
             QString("talk.google.com"));
    QCOMPARE(account->valueAsInt("parameters/port"), 5223);
    SettingSource source;
    QCOMPARE(account->valueAsInt("parameters/port", 0, &source), 5223);
    QCOMPARE(source, TEMPLATE);

    delete account;
    delete mgr;
}


void AccountsTest::testAccountEnabled()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account != 0);
    QSignalSpy enabled(account, SIGNAL(enabledChanged(const QString&,bool)));

    account->setEnabled(true);
    account->sync();
    QVERIFY(account->enabled());
    QTRY_COMPARE(enabled.count(), 1);
    QCOMPARE(enabled.at(0).at(1).toBool(), true);
    enabled.clear();
    account->setEnabled(false);
    account->sync();
    QVERIFY(!account->enabled());
    QTRY_COMPARE(enabled.count(), 1);
    QCOMPARE(enabled.at(0).at(1).toBool(), false);

    delete account;
    delete mgr;
}

void AccountsTest::testAccountDisplayName()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account != 0);
    QSignalSpy displayNameChanged(account,
                                  SIGNAL(displayNameChanged(const QString&)));

    account->setDisplayName(PROVIDER);
    account->sync();
    QCOMPARE(account->displayName(),PROVIDER);
    QTRY_COMPARE(displayNameChanged.count(), 1);
    QCOMPARE(displayNameChanged.at(0).at(0).toString(), PROVIDER);

    delete account;
    delete mgr;
}



void AccountsTest::testAccountValue()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account != 0);

    QSignalSpy synced(account, SIGNAL(synced()));
    account->setValue(QString("test"),QString("value"));
    int int_value = 666;
    account->setValue("testint", int_value);
    QStringList names;
    names << "Tom" << "Dick" << "Harry";
    account->setValue("names", names);
    account->setValue("big distance", quint64(3));
    account->setValue("big difference", qint64(-300));
    account->setValue("boolean", false);
    account->sync();

    QTRY_COMPARE(synced.count(), 1);

    /* check that the values we wrote are retrieved successfully */
    QVariant val = QVariant::String;
    account->value(QString("test"), val);
    QCOMPARE(val.toString(), QString("value"));

    SettingSource source;
    source = account->value(QString("test"), val);
    QCOMPARE(val.toString(), QString("value"));

    QVariant intval = QVariant::Int;
    account->value("testint", intval);
    qDebug("Val: %d", intval.toInt());
    QVERIFY(intval.toInt() == int_value);

    QVERIFY(source == ACCOUNT);

    QCOMPARE(account->value("names").toStringList(), names);
    QCOMPARE(account->value("big distance").toULongLong(), quint64(3));
    QCOMPARE(account->value("big difference").toLongLong(), qint64(-300));

    /* test the convenience methods */
    QString strval = account->valueAsString("test");
    QCOMPARE (strval, QString("value"));
    QCOMPARE(account->valueAsString("test", "Hi", &source), QString("value"));
    QCOMPARE(source, ACCOUNT);

    QCOMPARE(account->valueAsBool("boolean"), false);
    QCOMPARE(account->valueAsBool("boolean", true, &source), false);
    QCOMPARE(source, ACCOUNT);

    QCOMPARE(account->valueAsUInt64("big distance"), quint64(3));
    QCOMPARE(account->valueAsUInt64("big distance", 10, &source), quint64(3));
    QCOMPARE(source, ACCOUNT);

    strval = account->valueAsString("test_unexisting", "hello");
    QCOMPARE (strval, QString("hello"));

    delete account;
    delete mgr;
}

void AccountsTest::testAccountSync()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account != 0);

    QString provider = account->providerName();
    QCOMPARE(provider, PROVIDER);

    QSignalSpy synced(account, SIGNAL(synced()));

    account->sync();
    QTRY_COMPARE(synced.count(), 1);

    delete account;
    delete mgr;
}

void AccountsTest::testCreated()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    QSignalSpy created(mgr, SIGNAL(accountCreated(Accounts::AccountId)));

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);

    /* store the account: this will emit accountCreated */
    account->sync();
    QTRY_COMPARE(created.count(), 1);
    uint accountId = created.at(0).at(0).toUInt();
    QVERIFY(accountId != 0);
    QCOMPARE(accountId, account->id());

    delete account;
    delete mgr;
}

void AccountsTest::testRemove()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);

    /* store the account */
    account->sync();
    QVERIFY(account->id() != 0);

    QSignalSpy removed(account, SIGNAL(removed()));

    /* mark the account for deletion -- and make sure it's not deleted
     * immediately */
    account->remove ();
    QCOMPARE(removed.count(), 0);

    /* store the changes */
    account->sync();
    QCOMPARE(removed.count(), 1);

    delete account;
    delete mgr;
}

void AccountsTest::onAccountServiceChanged()
{
    AccountService *accountService = qobject_cast<AccountService*>(sender());
    m_accountServiceChangedFields = accountService->changedFields();
}

void AccountsTest::testAccountService()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);

    QObject *parent = new QObject();
    QPointer<AccountService> shortLivedAccountService =
        new AccountService(account, service, parent);
    QVERIFY(shortLivedAccountService != 0);
    delete parent;
    QVERIFY(shortLivedAccountService == 0);

    AccountService *accountService = new AccountService(account, service);
    QVERIFY(accountService != 0);

    QCOMPARE(accountService->account(), account);
    QCOMPARE(accountService->account()->providerName(),
             account->providerName());
    Service copy = accountService->service();
    QVERIFY(copy.isValid());
    QCOMPARE(copy.name(), service.name());

    QObject::connect(accountService, SIGNAL(changed()),
                     this, SLOT(onAccountServiceChanged()));
    QSignalSpy spyChanged(accountService, SIGNAL(changed()));
    QSignalSpy spyEnabled(accountService, SIGNAL(enabled(bool)));

    accountService->beginGroup("parameters");
    QVERIFY(accountService->group() == "parameters");

    /* test values from the template */
    QCOMPARE(accountService->value("server").toString(),
             UTF8("talk.google.com"));
    QCOMPARE(accountService->value("port").toInt(), 5223);
    QCOMPARE(accountService->value("old-ssl").toBool(), true);

    /* now, change some values */
    accountService->setValue("server", QString("www.example.com"));
    account->selectService();
    account->setEnabled(true);
    account->selectService(service);
    account->setEnabled(true);

    /* write the data */
    account->sync();

    /* ensure that the callbacks have been called the correct number of times */
    QCOMPARE(spyChanged.count(), 1);
    spyChanged.clear();
    spyEnabled.clear();

    QStringList expectedChanges;
    expectedChanges << "parameters/server";
    expectedChanges << "enabled";
    QCOMPARE(m_accountServiceChangedFields.toSet(), expectedChanges.toSet());

    QCOMPARE(accountService->value("server").toString(),
             UTF8("www.example.com"));
    QCOMPARE(accountService->enabled(), true);

    /* check the enabled status */
    account->selectService();
    account->setEnabled(false);
    account->sync();
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(spyEnabled.count(), 1);
    QCOMPARE(accountService->enabled(), false);
    QCOMPARE(spyEnabled.at(0).at(0).toBool(), accountService->enabled());
    spyEnabled.clear();

    /* enable the account, but disable the service */
    account->selectService();
    account->setEnabled(true);
    account->selectService(service);
    account->setEnabled(false);
    account->sync();
    QCOMPARE(spyEnabled.count(), 0);
    QCOMPARE(accountService->enabled(), false);

    /* re-enable the service */
    account->selectService(service);
    account->setEnabled(true);
    account->sync();
    QCOMPARE(spyEnabled.count(), 1);
    QCOMPARE(accountService->enabled(), true);
    QCOMPARE(spyEnabled.at(0).at(0).toBool(), accountService->enabled());
    spyEnabled.clear();
    spyChanged.clear();


    /* test some more APIs */
    QStringList expectedList;
    expectedList << "server" << "fallback-conference-server" <<
        "port" << "old-ssl";
    QCOMPARE(accountService->childKeys().toSet(), expectedList.toSet());
    QCOMPARE(accountService->childGroups().toSet(), QSet<QString>());
    QCOMPARE(accountService->contains("port"), true);
    accountService->endGroup();

    expectedList.clear();
    expectedList << "parameters";
    QCOMPARE(accountService->childGroups().toSet(), expectedList.toSet());

    /* Remove one key */
    accountService->remove("parameters/port");
    account->sync();
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(m_accountServiceChangedFields, QStringList("parameters/port"));
    spyChanged.clear();

    /* remove all keys */
    accountService->clear();
    account->sync();
    QCOMPARE(spyChanged.count(), 1);
    QVERIFY(m_accountServiceChangedFields.contains("parameters/server"));
    QVERIFY(m_accountServiceChangedFields.contains("parameters/old-ssl"));
    spyChanged.clear();

    delete accountService;
    delete account;
    delete mgr;
}

void AccountsTest::testWatches()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    /* create an account and some watches */
    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);

    account->selectService(service);

    Watch *w_server = account->watchKey("parameters/server");
    QSignalSpy serverNotify(w_server, SIGNAL(notify(const char *)));

    /* test also beginGroup() */
    account->beginGroup("parameters");
    QVERIFY(account->group() == "parameters");

    Watch *w_parameters = account->watchKey(); /* watch the whole group */
    QSignalSpy parametersNotify(w_parameters, SIGNAL(notify(const char *)));

    Watch *w_port = account->watchKey("port");
    QSignalSpy portNotify(w_port, SIGNAL(notify(const char *)));

    /* now, change some values */
    account->setValue("server", QString("xxx.nokia.com"));

    /* write the data */
    account->sync();

    /* ensure that the callbacks have been called the correct number of times */
    QCOMPARE(serverNotify.count(), 1);
    QCOMPARE(parametersNotify.count(), 1);
    QCOMPARE(portNotify.count(), 0);

    /* now change the port and another parameter not being watched */
    account->setValue("port", 45);
    account->endGroup(); /* exit from the "parameters/" prefix */
    account->setValue("username", QString("h4ck3r@nokia.com"));

    /* reset the counters */
    serverNotify.clear();
    parametersNotify.clear();
    portNotify.clear();

    /* write the data */
    account->sync();

    /* ensure that the callbacks have been called the correct number of times */
    QCOMPARE(serverNotify.count(), 0);
    QCOMPARE(parametersNotify.count(), 1);
    QCOMPARE(portNotify.count(), 1);

    /* change port and server, but delete the w_port watch */
    delete w_port;

    account->beginGroup("parameters");
    account->setValue("port", 56);
    account->setValue("server", QString("warez.nokia.com"));

    /* reset the counters */
    serverNotify.clear();
    parametersNotify.clear();
    portNotify.clear();

    /* write the data */
    account->sync();

    /* ensure that the callbacks have been called the correct number of times */
    QCOMPARE(serverNotify.count(), 1);
    QCOMPARE(parametersNotify.count(), 1);
    QCOMPARE(portNotify.count(), 0);

    delete account;
    delete mgr;
}

void AccountsTest::testServiceData()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    QDomDocument doc = service.domDocument();
    QVERIFY(!doc.isNull());

    delete mgr;
}

void AccountsTest::testSettings()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    /* create an account and some watches */
    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);

    /* create a few keys/groups */
    account->setValue("username", QString("fool"));
    account->setValue("email", QString("fool@domain.com"));

    account->beginGroup("parameters");
    account->setValue("server", QString("xxx.nokia.com"));
    account->setValue("port", 80);

    account->beginGroup("proxy"); /* this is a subgroup of "parameters" */
    account->setValue("use", true);
    account->setValue("address", QString("proxy.nokia.com"));
    QCOMPARE(account->group(), QString("parameters/proxy"));
    account->endGroup();

    QCOMPARE(account->group(), QString("parameters"));
    account->endGroup();

    /* write another top-level key */
    account->setValue("verified", false);

    /* re-enter a group */
    account->beginGroup("preferences");
    account->setValue("maximized", true);
    account->setValue("initial_directory", QString("xxx"));
    QCOMPARE(account->group(), QString("preferences"));
    account->endGroup();

    /* write the data */
    account->sync();

    /* now check if the groups and keys are layed out as expected */
    QStringList expected;
    QStringList result;

    /* check allKeys() */
    expected << "username" << "email" << "verified" <<
        "parameters/server" << "parameters/port" <<
        "parameters/proxy/use" << "parameters/proxy/address" <<
        "preferences/maximized" << "preferences/initial_directory";
    result = account->allKeys();

    expected.sort();
    result.sort();
    QCOMPARE(result, expected);

    /* check childGroups() */
    expected.clear();
    expected << "parameters" << "preferences";
    result = account->childGroups();

    expected.sort();
    result.sort();
    QCOMPARE(result, expected);

    /* check childKeys() */
    expected.clear();
    expected << "username" << "email" << "verified";
    result = account->childKeys();

    expected.sort();
    result.sort();
    QCOMPARE(result, expected);

    /* now enter a group and test the same methods as above */
    account->beginGroup("parameters");

    /* allKeys */
    expected.clear();
    expected << "server" << "port" << "proxy/use" << "proxy/address";
    result = account->allKeys();

    expected.sort();
    result.sort();
    QCOMPARE(result, expected);

    /* childGroups */
    expected.clear();
    expected << "proxy";
    result = account->childGroups();

    expected.sort();
    result.sort();
    QCOMPARE(result, expected);

    /* childKeys() */
    expected.clear();
    expected << "server" << "port";
    result = account->childKeys();

    expected.sort();
    result.sort();
    QCOMPARE(result, expected);

    /* contains() */
    QVERIFY(account->contains("server") == true);
    QVERIFY(account->contains("unexisting") == false);

    /* test remove() */
    account->remove(""); /* remove all the parameters/ group */
    account->endGroup();
    account->remove("verified");

    /* we need to sync, for the changes to take effect */
    account->sync();

    expected.clear();
    expected << "username" << "email" <<
        "preferences/maximized" << "preferences/initial_directory";
    result = account->allKeys();

    expected.sort();
    result.sort();
    QCOMPARE(result, expected);

    /* Test clear(): call it from inside the preferences/ group */
    account->beginGroup("preferences");
    account->clear();
    account->sync();

    expected.clear(); /* all keys should have been deleted */
    result = account->allKeys();

    expected.sort();
    result.sort();
    QCOMPARE(result, expected);

    delete account;
    delete mgr;
}

void AccountsTest::testKeySignVerify()
{
#ifndef HAVE_AEGISCRYPTO
    QSKIP("aegis-crypto not detected.");
#endif

    const QString key = "key";
    const char *token;
    QList<const char*> listOfTokens;
    listOfTokens << "libaccounts-glib0::accounts-glib-access"
                 << "libaccounts-glib0::dummy"
                 << "libaccounts-glib0::idiotic";
    bool ok;

    Manager *mgr = new Manager();
    QVERIFY (mgr != 0);

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);

    account->setValue(key, QString("the key value"));
    account->syncAndBlock();

    account->sign(key, listOfTokens.at(0));
    account->syncAndBlock();

    ok = account->verify(key, &token);
    QVERIFY(ok == true);

    ok = account->verifyWithTokens(key, listOfTokens);
    QVERIFY(ok == true);

    delete mgr;
}

void AccountsTest::testIncrementalAccountIds()
{
    clearDb();

    Manager *mgr = new Manager;
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);
    QVERIFY(account->manager() == mgr);
    account->syncAndBlock();
    AccountId lastId = account->id();

    account->remove();
    account->syncAndBlock();
    delete account;

    account = mgr->createAccount(NULL);
    QVERIFY(account != 0);
    QVERIFY(account->manager() == mgr);
    account->syncAndBlock();

    QCOMPARE(account->id(), lastId + 1);

    delete account;
    delete mgr;
}

void AccountsTest::testSelectGlobalAccountSettings()
{
    Manager *mgr = new Manager;
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != 0);
    QVERIFY(account->manager() == mgr);

    account->syncAndBlock();
    QVERIFY(account->id() > 0);

    account->selectService();
    Service selectedService = account->selectedService();
    QVERIFY(!selectedService.isValid());

    delete mgr;
}

void AccountsTest::testCredentialsId()
{
    Manager *mgr = new Manager;
    QVERIFY(mgr != 0);

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != 0);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    uint globalId = 69, myServiceId = 0xDEAD;

    account->setCredentialsId(globalId);

    account->selectService(service);
    account->setCredentialsId(myServiceId);

    account->syncAndBlock();
    QVERIFY(account->id() != 0);

    QCOMPARE(account->credentialsId(), myServiceId);

    /* select a service with no credentials: we should get the global
     * credentials ID, but the selected service shouldn't change */
    service = mgr->service(OTHERSERVICE);
    QVERIFY(service.isValid());

    account->selectService(service);
    QCOMPARE(account->credentialsId(), globalId);
    QCOMPARE(account->selectedService(), service);

    /* now make sure that we can get the ID from the global accounts settings */
    account->selectService();
    QCOMPARE(account->credentialsId(), globalId);

    delete mgr;
}

void AccountsTest::testAuthData()
{
    Manager *manager = new Manager;
    QVERIFY(manager != 0);

    Account *account = manager->createAccount("MyProvider");
    QVERIFY(account != 0);

    Service service = manager->service(MYSERVICE);
    QVERIFY(service.isValid());

    const uint credentialsId = 69;
    const QString method = "mymethod";
    const QString mechanism = "mymechanism";
    QString prefix =
        QString::fromLatin1("auth/%1/%2/").arg(method).arg(mechanism);

    QVariantMap globalParameters;
    globalParameters["server"] = UTF8("myserver.com");
    globalParameters["port"] = 8080;
    globalParameters["other"] = UTF8("overriden parameter");
    globalParameters["scopes"] = QStringList() << "read" << "write" << "edit";

    QVariantMap serviceParameters;
    serviceParameters["other"] = UTF8("better parameter");
    serviceParameters["boolean"] = true;

    account->setCredentialsId(credentialsId);
    account->setValue("auth/method", method);
    account->setValue("auth/mechanism", UTF8("overriden mechanism"));
    QMapIterator<QString,QVariant> i(globalParameters);
    while (i.hasNext()) {
        i.next();
        account->setValue(prefix + i.key(), i.value());
    }

    account->selectService(service);
    account->setValue("auth/mechanism", mechanism);
    i = QMapIterator<QString,QVariant>(serviceParameters);
    while (i.hasNext()) {
        i.next();
        account->setValue(prefix + i.key(), i.value());
    }

    account->syncAndBlock();
    QVERIFY(account->id() != 0);

    AccountService *accountService = new AccountService(account, service);
    QVERIFY(accountService != 0);

    AuthData authData = accountService->authData();
    QCOMPARE(authData.method(), method);
    QCOMPARE(authData.mechanism(), mechanism);
    QCOMPARE(authData.credentialsId(), credentialsId);

    QVariantMap expectedParameters = globalParameters;
    i = QMapIterator<QString,QVariant>(serviceParameters);
    while (i.hasNext()) {
        i.next();
        expectedParameters.insert(i.key(), i.value());
    }

    QCOMPARE(authData.parameters(), expectedParameters);

    /* Test copy constructor */
    AuthData copy(authData);
    QCOMPARE(copy.parameters(), expectedParameters);

    /* And delete destructor */
    AuthData *copy2 = new AuthData(authData);
    QCOMPARE(copy2->parameters(), expectedParameters);
    delete copy2;

    delete accountService;
    delete account;
    delete manager;
    QVERIFY(service.isValid());
}

void AccountsTest::testGlobalAuthData()
{
    Manager *manager = new Manager;
    QVERIFY(manager != 0);

    Account *account = manager->createAccount("MyProvider");
    QVERIFY(account != 0);

    Service service; // global account
    QVERIFY(!service.isValid());

    AccountService *accountService = new AccountService(account, service);
    QVERIFY(accountService != 0);

    AuthData authData = accountService->authData();
    QCOMPARE(authData.method(), QString("oauth2"));
    QCOMPARE(authData.mechanism(), QString("user_agent"));
    QCOMPARE(authData.credentialsId(), uint(0));

    QVariantMap expectedParameters;
    expectedParameters.insert("Host", QString("myserver.example"));

    QCOMPARE(authData.parameters(), expectedParameters);

    delete accountService;
    delete account;
    delete manager;
}

void AccountsTest::testListEnabledServices()
{
    clearDb();

    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    Account* account = mgr->createAccount("MyProvider");
    QVERIFY(account != 0);
    account->selectService(service);
    account->setEnabled(true);
    account->sync();

    ServiceList list = account->enabledServices();
    QVERIFY(!list.isEmpty());
    QCOMPARE(list.size(), 1);

    account->selectService(service);
    account->setEnabled(false);
    account->sync();

    list = account->enabledServices();
    QVERIFY(list.isEmpty());

    delete account;
    delete mgr;
}


void AccountsTest::testListEnabledByServiceType()
{
    clearDb();

    Manager *mgr = new Manager("e-mail");
    QVERIFY(mgr != 0);
    QCOMPARE(mgr->serviceType(), QString("e-mail"));

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != 0);
    account->setEnabled(true);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());
    account->selectService(service);
    account->setEnabled(true);
    account->sync();

    AccountIdList list = mgr->accountListEnabled("e-mail");
    QVERIFY(!list.isEmpty());
    QCOMPARE(list.size(), 1);

    list = mgr->accountListEnabled();
    QCOMPARE(list.count(), 1);

    account->setEnabled(false);
    account->sync();

    list = mgr->accountListEnabled("e-mail");
    QVERIFY(list.isEmpty());

    delete account;
    delete mgr;
}

void AccountsTest::testEnabledEvent()
{
    Manager *mgr = new Manager("e-mail");
    QVERIFY(mgr != 0);

    QSignalSpy enabledEvent1(mgr, SIGNAL(enabledEvent(Accounts::AccountId)));

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != 0);
    account->setEnabled(true);
    account->sync();

    QTRY_COMPARE(enabledEvent1.count(), 1);
    QCOMPARE(enabledEvent1.at(0).at(0).toUInt(), account->id());

    //if we create manager without service type the signal should not be emitted
    Manager *mgr2 = new Manager();
    QVERIFY(mgr2 != 0);

    QSignalSpy enabledEvent2(mgr2, SIGNAL(enabledEvent(Accounts::AccountId)));

    Account *account2 = mgr2->createAccount(NULL);
    QVERIFY(account2 != 0);
    account2->setEnabled(true);
    account2->sync();

    QCOMPARE(enabledEvent2.count(), 0);

    delete account;
    delete account2;
    delete mgr;
    delete mgr2;
}

void AccountsTest::testServiceType()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != 0);

    ServiceType serviceType;

    serviceType = mgr->serviceType("unexisting-type");
    QVERIFY(!serviceType.isValid());

    serviceType = mgr->serviceType(EMAIL_SERVICE_TYPE);
    QVERIFY(serviceType.isValid());

    QCOMPARE(serviceType.name(), EMAIL_SERVICE_TYPE);
    QCOMPARE(serviceType.description(), QStringLiteral("Email test description"));
    QCOMPARE(serviceType.displayName(), QStringLiteral("Electronic mail"));
    QCOMPARE(serviceType.trCatalog(), QStringLiteral("translation_file"));
    QCOMPARE(serviceType.iconName(), QStringLiteral("email_icon"));
    QVERIFY(serviceType.tags().contains(QString("email")));
    // called twice, because the second time it returns a cached result
    QVERIFY(serviceType.tags().contains(QString("email")));
    QVERIFY(serviceType.hasTag(QString("email")));
    QVERIFY(!serviceType.hasTag(QString("fake-email")));

    QDomDocument dom = serviceType.domDocument();
    QDomElement root = dom.documentElement();
    QCOMPARE(root.tagName(), QString("service-type"));

    ServiceType copy(serviceType);
    QCOMPARE(copy.displayName(), QStringLiteral("Electronic mail"));

    delete mgr;
}

void AccountsTest::testUpdateAccount()
{
    clearDb();

    Manager *mgr = new Manager("e-mail");
    QVERIFY(mgr != 0);

    QSignalSpy accountUpdated(mgr,
                              SIGNAL(accountUpdated(Accounts::AccountId)));

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != 0);
    account->syncAndBlock();

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());
    account->selectService(service);

    account->setValue("key", QVariant("value"));
    account->syncAndBlock();

    QTRY_COMPARE(accountUpdated.count(), 1);
    QCOMPARE(accountUpdated.at(0).at(0).toUInt(), account->id());

    //if we create manager without service type the signal shoudl not be emitted
    Manager *mgr2 = new Manager();
    QVERIFY(mgr2 != 0);

    QSignalSpy accountUpdated2(mgr2,
                               SIGNAL(accountUpdated(Accounts::AccountId)));

    Account *account2 = mgr2->createAccount(NULL);
    QVERIFY(account2 != 0);

    account2->setValue("key", QVariant("value"));
    account2->syncAndBlock();

    QCOMPARE(accountUpdated2.count(), 0);

    delete account;
    delete account2;
    delete mgr;
    delete mgr2;
}

void AccountsTest::testApplication()
{
    Manager *manager = new Manager();
    QVERIFY(manager != 0);

    Application application = manager->application("Mailer");
    QVERIFY(application.isValid());

    Service email = manager->service("MyService");
    QVERIFY(email.isValid());

    Service sharing = manager->service("OtherService");
    QVERIFY(sharing.isValid());

    QCOMPARE(application.name(), UTF8("Mailer"));
    QCOMPARE(application.displayName(), UTF8("Easy Mailer"));
    QCOMPARE(application.description(), UTF8("Mailer application"));
    QCOMPARE(application.trCatalog(), UTF8("mailer-catalog"));
    QCOMPARE(application.iconName(), UTF8("mailer-icon"));
    QCOMPARE(application.desktopFilePath(),
             UTF8(qgetenv("AG_APPLICATIONS") + "/applications/mailer.desktop"));
    QVERIFY(application.supportsService(email));
    QCOMPARE(application.serviceUsage(email),
             UTF8("Mailer can retrieve your e-mails"));

    ApplicationList apps = manager->applicationList(email);
    QCOMPARE(apps.count(), 1);
    QCOMPARE(apps[0].name(), UTF8("Mailer"));

    apps = manager->applicationList(sharing);
    QCOMPARE(apps.count(), 1);
    application = apps[0];
    QCOMPARE(application.name(), UTF8("Gallery"));
    QCOMPARE(application.description(), UTF8("Image gallery"));
    QVERIFY(application.supportsService(sharing));
    QCOMPARE(application.serviceUsage(sharing),
             UTF8("Publish images on OtherService"));

    /* Test an invalid application */
    Application app2;
    QVERIFY(!app2.isValid());
    Application app3(app2);
    QVERIFY(!app3.isValid());

    delete manager;
}

void AccountsTest::testApplicationListServices()
{
    Manager *manager = new Manager();
    QVERIFY(manager != 0);

    Application application = manager->application("Mailer");
    QVERIFY(application.isValid());

    ServiceList services = manager->serviceList(application);

    QCOMPARE(services.count(), 1);
    Service service = services.first();

    QCOMPARE(service.name(), QString("MyService"));

    delete manager;
}

QTEST_GUILESS_MAIN(AccountsTest)
#include "tst_libaccounts.moc"
