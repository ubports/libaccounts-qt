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
#include <QtTest/QtTest>
#include <QSignalSpy>

#include "Accounts/Application"
#include "Accounts/Manager"
#include "Accounts/AccountService"

using namespace Accounts;
#include "accountstest.h"

#define PROVIDER QString("dummyprovider")
#define SERVICE QString("dummyservice")
#define MYSERVICE QLatin1String("MyService")
#define OTHERSERVICE QLatin1String("OtherService")
#define EMAIL_SERVICE_TYPE QLatin1String("e-mail")


void clearDb()
{
    QDir dbroot(QString(getenv("ACCOUNTS")));
    dbroot.remove(QString("accounts.db"));
}

void AccountsTest::initTestCase()
{
    //init
    setenv ("ACCOUNTS", "/tmp/", false);
    setenv ("AG_SERVICES", SERVICES_DIR, false);
    setenv ("AG_SERVICE_TYPES", SERVICE_TYPES_DIR, false);
    setenv ("AG_PROVIDERS", PROVIDERS_DIR, false);
    //clear database
    clearDb();
}

void AccountsTest::cleanupTestCase()
{
}


/* manager */
void AccountsTest::managerTestCase()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != NULL);
    QCOMPARE(mgr->serviceType(), QString());

    mgr->setAbortOnTimeout(true);
    QCOMPARE(mgr->abortOnTimeout(), true);

    mgr->setTimeout(123);
    QCOMPARE(mgr->timeout(), quint32(123));

    delete mgr;
}

void AccountsTest::createAccountTestCase()
{
    clearDb();

    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account!=NULL);
    QVERIFY(account->manager() == mgr);
    delete account;

    account = mgr->createAccount(PROVIDER);
    QVERIFY(account!=NULL);
    delete account;

    delete mgr;
}

void AccountsTest::accountTestCase()
{
    clearDb();

    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    //this should not exist
    Account* account2 = mgr->account(1);
    QVERIFY(account2==NULL);
    Error error = mgr->lastError();
    QCOMPARE(error.type(), Error::AccountNotFound);

    // make sure there is account
    Account* account = mgr->createAccount(NULL);
    account->sync();

    //now we get account
    account2 = mgr->account(1);
    QVERIFY(account2!=NULL);

    delete account2;
    delete account;
    delete mgr;
}

void AccountsTest::objectsLifetimeTestCase()
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

void AccountsTest::accountListTestCase()
{
    clearDb();

    Manager *mgr = new Manager();
    QVERIFY(mgr != NULL);

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

void AccountsTest::providerTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account!=NULL);

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
    QCOMPARE(provider.trCatalog(), QString("accounts"));

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

void AccountsTest::serviceTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());
    QCOMPARE(service.name(), MYSERVICE);
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

void AccountsTest::serviceListTestCase()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != NULL);

    ServiceList list = mgr->serviceList();
    QVERIFY(!list.isEmpty());
    QCOMPARE(list.count(), 2);

    list = mgr->serviceList("e-mail");
    QCOMPARE(list.count(), 1);

    list = mgr->serviceList("sharing");
    QCOMPARE(list.count(), 1);

    delete mgr;
}

/* service */

void AccountsTest::serviceConstTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());
    QCOMPARE(service.name(), MYSERVICE);
    QCOMPARE(service.displayName(), QString("My Service"));
    QCOMPARE(service.serviceType(), QString("e-mail"));
    QCOMPARE(service.provider(), QString("MyProvider"));
    QVERIFY(service.hasTag(QString("messaging")));

    delete mgr;
}


/* account */


void AccountsTest::accountConstTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account!=NULL);
    QVERIFY(account->isWritable());

    delete account;
    delete mgr;
}

void AccountsTest::accountProviderTestCase()
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

void AccountsTest::accountServiceTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account!=NULL);

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


void AccountsTest::enabled(const QString & /*serviceName*/, bool enabled)
{
    m_enabled = enabled;
}


void AccountsTest::accountEnabledTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account!=NULL);
    m_enabled=false;
    QObject::connect(account, SIGNAL(enabledChanged(const QString&,bool)),
                      this,  SLOT(enabled(const QString&,bool)));

    account->setEnabled(true);
    account->sync();
    QVERIFY(account->enabled());
    QVERIFY(m_enabled);
    account->setEnabled(false);
    account->sync();
    QVERIFY(!account->enabled());
    QVERIFY(!m_enabled);

    delete account;
    delete mgr;
}

void AccountsTest::display(const QString &displayName)
{
    m_enabled = !displayName.isNull();
}

void AccountsTest::accountDisplayNameTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account!=NULL);
    m_enabled=false;
    QObject::connect(account, SIGNAL(displayNameChanged(const QString&)),
                      this,  SLOT(display(const QString&)));

    account->setDisplayName(PROVIDER);
    account->sync();
    QCOMPARE(account->displayName(),PROVIDER);
    QVERIFY(m_enabled);

    delete account;
    delete mgr;
}



void AccountsTest::accountValueTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account!=NULL);

    QObject::connect(account, SIGNAL(synced()),
                      this,  SLOT(stored()));
    m_stored = false;
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

    QTest::qWait(10);
    QVERIFY(m_stored);

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


void AccountsTest::stored()
{
    qDebug("%s %d %s:\t", __FILE__, __LINE__, __func__);
    TRACE();
    m_stored = true;
}

void AccountsTest::error(Accounts::Error error)
{
    qDebug("%s %d %s:\t", __FILE__, __LINE__, __func__);
    TRACE() << "error:" << error.type();
    m_stored = false;
}


void AccountsTest::accountSyncTestCase()
{
    Manager * mgr=new Manager();
    QVERIFY(mgr!=NULL);

    Account *account = mgr->createAccount(PROVIDER);
    QVERIFY(account!=NULL);

    QString provider = account->providerName();
    QCOMPARE(provider, PROVIDER);

    m_stored = false;
    QObject::connect(account, SIGNAL(synced()),
                      this,  SLOT(stored()));

    account->sync();
    QTest::qWait(10);
    QVERIFY(m_stored);

    delete account;
    delete mgr;
}

void AccountsTest::created(Accounts::AccountId id)
{
    m_created = id;
}

void AccountsTest::createdTestCase()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != NULL);

    m_created = 0;
    QObject::connect(mgr, SIGNAL(accountCreated(Accounts::AccountId)),
                     this, SLOT(created(Accounts::AccountId)));

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);

    /* store the account: this will emit accountCreated */
    account->sync();
    QVERIFY(m_created != 0);
    QVERIFY(m_created == account->id());

    delete account;
    delete mgr;
}

void AccountsTest::removed()
{
    m_removed = true;
}

void AccountsTest::removeTestCase()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != NULL);

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);

    /* store the account */
    account->sync();
    QVERIFY(account->id() != 0);

    m_removed = false;
    QObject::connect(account, SIGNAL(removed()), this, SLOT(removed()));

    /* mark the account for deletion -- and make sure it's not deleted
     * immediately */
    account->remove ();
    QVERIFY(m_removed == false);

    /* store the changes */
    account->sync();
    QVERIFY(m_removed == true);

    delete account;
    delete mgr;
}

void AccountsTest::w_server_notify(const char *key)
{
    qDebug() << __func__ << ", key = " << key;
    m_server_notify++;
}

void AccountsTest::w_port_notify(const char *key)
{
    qDebug() << __func__ << ", key = " << key;
    m_port_notify++;
}

void AccountsTest::w_parameters_notify(const char *key)
{
    qDebug() << __func__ << ", key = " << key;
    m_parameters_notify++;
}

void AccountsTest::onAccountServiceEnabled(bool enabled)
{
    qDebug() << Q_FUNC_INFO;
    m_accountServiceEnabledValue = enabled;
}

void AccountsTest::onAccountServiceChanged()
{
    qDebug() << Q_FUNC_INFO;
    AccountService *accountService = qobject_cast<AccountService*>(sender());
    m_accountServiceChangedFields = accountService->changedFields();
}

void AccountsTest::accountServiceTest()
{
    Manager *mgr = new Manager();
    QVERIFY (mgr != NULL);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);

    QObject *parent = new QObject();
    QPointer<AccountService> shortLivedAccountService =
        new AccountService(account, service, parent);
    QVERIFY(shortLivedAccountService != 0);
    delete parent;
    QVERIFY(shortLivedAccountService == 0);

    AccountService *accountService = new AccountService(account, service);
    QVERIFY(accountService != NULL);

    QCOMPARE(accountService->account(), account);
    QCOMPARE(accountService->account()->providerName(),
             account->providerName());
    Service copy = accountService->service();
    QVERIFY(copy.isValid());
    QCOMPARE(copy.name(), service.name());

    QObject::connect(accountService, SIGNAL(changed()),
                     this, SLOT(onAccountServiceChanged()));
    QObject::connect(accountService, SIGNAL(enabled(bool)),
                     this, SLOT(onAccountServiceEnabled(bool)));
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
    QCOMPARE(m_accountServiceEnabledValue, accountService->enabled());
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
    QCOMPARE(m_accountServiceEnabledValue, accountService->enabled());
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

void AccountsTest::watchesTest()
{
    Manager *mgr = new Manager();
    QVERIFY (mgr != NULL);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    /* create an account and some watches */
    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);

    account->selectService(service);

    Watch *w_server = account->watchKey("parameters/server");
    QObject::connect(w_server, SIGNAL(notify(const char *)),
                     this, SLOT(w_server_notify(const char *)));

    /* test also beginGroup() */
    account->beginGroup("parameters");
    QVERIFY(account->group() == "parameters");

    Watch *w_parameters = account->watchKey(); /* watch the whole group */
    QObject::connect(w_parameters, SIGNAL(notify(const char *)),
                     this, SLOT(w_parameters_notify(const char *)));

    Watch *w_port = account->watchKey("port");
    QObject::connect(w_port, SIGNAL(notify(const char *)),
                     this, SLOT(w_port_notify(const char *)));

    /* now, change some values */
    account->setValue("server", QString("xxx.nokia.com"));

    /* reset the counters */
    m_server_notify = 0;
    m_port_notify = 0;
    m_parameters_notify = 0;

    /* write the data */
    account->sync();

    /* ensure that the callbacks have been called the correct number of times */
    QVERIFY(m_server_notify == 1);
    QVERIFY(m_parameters_notify == 1);
    QVERIFY(m_port_notify == 0);

    /* now change the port and another parameter not being watched */
    account->setValue("port", 45);
    account->endGroup(); /* exit from the "parameters/" prefix */
    account->setValue("username", QString("h4ck3r@nokia.com"));

    /* reset the counters */
    m_server_notify = 0;
    m_port_notify = 0;
    m_parameters_notify = 0;

    /* write the data */
    account->sync();

    /* ensure that the callbacks have been called the correct number of times */
    QVERIFY(m_server_notify == 0);
    QVERIFY(m_parameters_notify == 1);
    QVERIFY(m_port_notify == 1);

    /* change port and server, but delete the w_port watch */
    delete w_port;

    account->beginGroup("parameters");
    account->setValue("port", 56);
    account->setValue("server", QString("warez.nokia.com"));

    /* reset the counters */
    m_server_notify = 0;
    m_port_notify = 0;
    m_parameters_notify = 0;

    /* write the data */
    account->sync();

    /* ensure that the callbacks have been called the correct number of times */
    QVERIFY(m_server_notify == 1);
    QVERIFY(m_parameters_notify == 1);
    QVERIFY(m_port_notify == 0);


    delete account;
    delete mgr;
}

void AccountsTest::serviceDataTest()
{
    Manager *mgr = new Manager();
    QVERIFY (mgr != NULL);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    QDomDocument doc = service.domDocument();
    QVERIFY(!doc.isNull());

    delete mgr;
}

void AccountsTest::settingsTest()
{
    Manager *mgr = new Manager();
    QVERIFY (mgr != NULL);

    /* create an account and some watches */
    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);

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

void AccountsTest::keySignVerifyTest()
{
#ifndef HAVE_AEGISCRYPTO
    QSKIP("aegis-crypto not detected.", SkipSingle);
#endif

    const QString key = "key";
    const char *token;
    QList<const char*> listOfTokens;
    listOfTokens << "libaccounts-glib0::accounts-glib-access"
                 << "libaccounts-glib0::dummy"
                 << "libaccounts-glib0::idiotic";
    bool ok;

    Manager *mgr = new Manager();
    QVERIFY (mgr != NULL);

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);

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

void AccountsTest::incrementalAccountIdsTest()
{
    clearDb();

    Manager * mgr = new Manager;
    QVERIFY(mgr != NULL);

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);
    QVERIFY(account->manager() == mgr);
    account->syncAndBlock();
    AccountId lastId = account->id();

    account->remove();
    account->syncAndBlock();
    delete account;

    account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);
    QVERIFY(account->manager() == mgr);
    account->syncAndBlock();

    QCOMPARE(account->id(), lastId + 1);

    delete account;
    delete mgr;
}

void AccountsTest::selectGlobalAccountSettingsTest()
{
    Manager *mgr = new Manager;
    QVERIFY(mgr != NULL);

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != NULL);
    QVERIFY(account->manager() == mgr);

    account->syncAndBlock();
    QVERIFY(account->id() > 0);

    account->selectService();
    Service selectedService = account->selectedService();
    QVERIFY(!selectedService.isValid());

    delete mgr;
}

void AccountsTest::credentialsIdTest()
{
    Manager *mgr = new Manager;
    QVERIFY(mgr != NULL);

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != NULL);

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

void AccountsTest::authDataTest()
{
    Manager *manager = new Manager;
    QVERIFY(manager != NULL);

    Account *account = manager->createAccount("MyProvider");
    QVERIFY(account != NULL);

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

void AccountsTest::globalAuthDataTest()
{
    Manager *manager = new Manager;
    QVERIFY(manager != NULL);

    Account *account = manager->createAccount("MyProvider");
    QVERIFY(account != NULL);

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

void AccountsTest::listEnabledServices()
{
    clearDb();

    Manager *mgr = new Manager();
    QVERIFY(mgr != NULL);

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());

    Account* account = mgr->createAccount("MyProvider");
    QVERIFY(account != NULL);
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


void AccountsTest::listEnabledByServiceType()
{
    clearDb();

    Manager *mgr = new Manager("e-mail");
    QVERIFY(mgr != NULL);
    QCOMPARE(mgr->serviceType(), QString("e-mail"));

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != NULL);
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

void AccountsTest::enabledEvent(Accounts::AccountId id)
{
    m_enabledEvent = id;
}

void AccountsTest::enabledEvent()
{
    Manager *mgr = new Manager("e-mail");
    QVERIFY(mgr != NULL);

    m_enabledEvent = 0;
    QObject::connect(mgr, SIGNAL(enabledEvent(Accounts::AccountId)),
                     this, SLOT(enabledEvent(Accounts::AccountId)));

    Account *account = mgr->createAccount(NULL);
    QVERIFY(account != NULL);
    account->setEnabled(true);
    account->sync();

    QVERIFY(m_enabledEvent != 0);
    QVERIFY(m_enabledEvent == account->id());

    //if we create manager without service type the signal shoudl not be emitted
    Manager *mgr2 = new Manager();
    QVERIFY(mgr2 != NULL);

    m_enabledEvent = 0;
    QObject::connect(mgr2, SIGNAL(enabledEvent(Accounts::AccountId)),
                     this, SLOT(enabledEvent(Accounts::AccountId)));

    Account *account2 = mgr2->createAccount(NULL);
    QVERIFY(account2 != NULL);
    account2->setEnabled(true);
    account2->sync();

    QVERIFY(m_enabledEvent == 0);
    QVERIFY(m_enabledEvent != account2->id());

    delete account;
    delete account2;
    delete mgr;
    delete mgr2;
}

void AccountsTest::serviceTypeTestCase()
{
    Manager *mgr = new Manager();
    QVERIFY(mgr != NULL);

    ServiceType serviceType;

    serviceType = mgr->serviceType("unexisting-type");
    QVERIFY(!serviceType.isValid());

    serviceType = mgr->serviceType(EMAIL_SERVICE_TYPE);
    QVERIFY(serviceType.isValid());

    QCOMPARE(serviceType.name(), EMAIL_SERVICE_TYPE);
    QCOMPARE(serviceType.displayName(), QLatin1String("Electronic mail"));
    QCOMPARE(serviceType.trCatalog(), QLatin1String("translation_file"));
    QCOMPARE(serviceType.iconName(), QLatin1String("email_icon"));
    QVERIFY(serviceType.tags().contains(QString("email")));
    // called twice, because the second time it returns a cached result
    QVERIFY(serviceType.tags().contains(QString("email")));
    QVERIFY(serviceType.hasTag(QString("email")));
    QVERIFY(!serviceType.hasTag(QString("fake-email")));

    QDomDocument dom = serviceType.domDocument();
    QDomElement root = dom.documentElement();
    QCOMPARE(root.tagName(), QString("service-type"));

    ServiceType copy(serviceType);
    QCOMPARE(copy.displayName(), QLatin1String("Electronic mail"));

    delete mgr;
}

void AccountsTest::updateAccountTestCase()
{
    clearDb();

    Manager *mgr = new Manager("e-mail");
    QVERIFY(mgr != NULL);

    m_updateEvent = 0;
    QObject::connect(mgr, SIGNAL(accountUpdated(Accounts::AccountId)),
                     this, SLOT(updateAccount(Accounts::AccountId)));

    Account *account = mgr->createAccount("MyProvider");
    QVERIFY(account != NULL);
    account->syncAndBlock();

    Service service = mgr->service(MYSERVICE);
    QVERIFY(service.isValid());
    account->selectService(service);

    account->setValue("key", QVariant("value"));
    account->syncAndBlock();

    QVERIFY(m_updateEvent != 0);
    QVERIFY(m_updateEvent == account->id());

    //if we create manager without service type the signal shoudl not be emitted
    Manager *mgr2 = new Manager();
    QVERIFY(mgr2 != NULL);

    m_updateEvent = 0;
    QObject::connect(mgr2, SIGNAL(accountUpdated(Accounts::AccountId)),
                     this, SLOT(updateAccount(Accounts::AccountId)));

    Account *account2 = mgr2->createAccount(NULL);
    QVERIFY(account2 != NULL);

    account2->setValue("key", QVariant("value"));
    account2->syncAndBlock();

    QVERIFY(m_updateEvent == 0);
    QVERIFY(m_updateEvent != account2->id());

    delete account;
    delete account2;
    delete mgr;
    delete mgr2;
}

void AccountsTest::updateAccount(Accounts::AccountId id)
{
    m_updateEvent = id;
}

void AccountsTest::applicationTest()
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
    QCOMPARE(application.description(), UTF8("Mailer application"));
    QCOMPARE(application.trCatalog(), UTF8("mailer-catalog"));
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
    QCOMPARE(application.serviceUsage(sharing),
             UTF8("Publish images on OtherService"));

    delete manager;
}

QTEST_MAIN(AccountsTest)
// #include "testqstring.moc"
