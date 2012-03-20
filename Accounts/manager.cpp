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

#include "application.h"
#include "service.h"
#include "manager.h"

#undef signals
#include <libaccounts-glib/ag-manager.h>
#include <libaccounts-glib/ag-account.h>


namespace Accounts {

/*!
 * @class Manager
 * @headerfile manager.h Accounts/Manager
 *
 * @brief Manager of accounts, services and providers.
 *
 * @details The Manager offers ways to create accounts, list accounts, services
 * and providers. It also emits signals when accounts are created and removed.
 */

/*!
 * @fn Manager::accountCreated(Accounts::AccountId id)
 *
 * The signal is emitted when new account is created.
 *
 * @param id Identifier of the Account
 */

/*!
 * @fn Manager::accountRemoved(Accounts::AccountId id)
 *
 * The signal is emitted when existing account is removed.
 *
 * @param id Identifier of the Account
 */

/*!
 * @fn Manager::accountUpdated(Accounts::AccountId id)
 *
 * The signal is emitted when any account property for a particular service is
 * updated.
 *
 * To receive this notification user has to create accounts manager using
 * Manager(const QString &serviceType, QObject *parent) constructor.
 * Update notification is only emitted when manager is created for particular
 * type of service.
 *
 * @param id Identifier of the Account
 */

/*!
 * @fn Manager::enabledEvent(Accounts::AccountId id)
 *
 * If the manager has been created with serviceType, this
 * signal will be emitted when an account (identified by AccountId) has been
 * modified in such a way that the application might be interested to
 * start/stop using it: the "enabled" flag on the account or in some service
 * supported by the account and matching the AgManager::serviceType have
 * changed.
 * @note In practice, this signal might be emitted more often than when
 * strictly needed; applications must call Account::enabledServices() to get
 * the current state.
 *
 * @param id identifier of the Account
 */

class Manager::Private
{
    Q_DECLARE_PUBLIC(Manager)

    typedef QHash<AgProvider *, Provider *> ProviderHash;
    typedef QHash<const QString, ServiceType *> ServiceTypeHash;
public:
    Private():
        q_ptr(0),
        m_manager(0),
        providers(),
        serviceTypes()
    {
    }

    ~Private() {
        foreach (Provider *provider, providers)
        {
            delete provider;
        }
        providers.clear();

        foreach (ServiceType *serviceType, serviceTypes)
        {
            delete serviceType;
        }
        serviceTypes.clear();
    }

    void init(Manager *q, AgManager *manager);

    mutable Manager *q_ptr;
    AgManager *m_manager; //real manager
    ProviderHash providers;
    ServiceTypeHash serviceTypes;
    Error lastError;

    static void on_account_created(Manager *self, AgAccountId id);
    static void on_account_deleted(Manager *self, AgAccountId id);
    static void on_account_updated(Manager *self, AgAccountId id);
    static void on_enabled_event(Manager *self, AgAccountId id);
};

} //namespace Accounts

using namespace Accounts;

void Manager::Private::init(Manager *q, AgManager *manager)
{
    Q_ASSERT(q_ptr == 0);
    Q_ASSERT(m_manager == 0);

    q_ptr = q;
    m_manager = manager;

    if (manager) {
        g_signal_connect_swapped
            (manager, "account-created",
             G_CALLBACK(&Private::on_account_created), q);
        g_signal_connect_swapped
            (manager, "account-deleted",
             G_CALLBACK(&Private::on_account_deleted), q);
        g_signal_connect_swapped
            (manager, "account-updated",
             G_CALLBACK(&Private::on_account_updated), q);
        g_signal_connect_swapped
            (manager, "enabled-event",
             G_CALLBACK(&Private::on_enabled_event), q);
    } else {
        qWarning() << Q_FUNC_INFO << "Initializing with NULL AgManager!";
    }
}

void Manager::Private::on_account_created(Manager *self, AgAccountId id)
{
    TRACE() << "id =" << id;

    emit self->accountCreated(id);
}

void Manager::Private::on_account_deleted(Manager *self, AgAccountId id)
{
    TRACE() << "id =" << id;

    emit self->accountRemoved(id);
}

void Manager::Private::on_account_updated(Manager *self, AgAccountId id)
{
    TRACE() << "id =" << id;

    emit self->accountUpdated(id);
}

void Manager::Private::on_enabled_event(Manager *self, AgAccountId id)
{
    TRACE() << "id =" << id;

    emit self->enabledEvent(id);
}

/*!
 * Constructor.
 * Users should check for manager->lastError() to check if manager construction
 * was fully succesful.
 */
Manager::Manager(QObject *parent)
    : QObject(parent), d(new Private)
{
    g_type_init();

    AgManager *manager = ag_manager_new();

    if (manager != 0) {
        d->init(this, manager);
    } else {
        qWarning() << "Manager could not be created. DB is locked";
        d->lastError = Error::DatabaseLocked;
    }

}

/*!
 * Constructs a manager initialized with service type. This constructor
 * should be used when there is an interest for just one service type.
 * Such a manager has influence on some class methods. When listing the
 * accounts and services only the ones supporting the given service type
 * will be returned. Also the creating account with this manager will
 * affect the acccount class method for listing services in same manner.
 * The signal enabledEvent() will be emitted only when manager is created
 * with this constructor.
 * Users should check for manager->lastError() to check if manager construction
 * was fully succesful.
 */
Manager::Manager(const QString &serviceType, QObject *parent)
    : QObject(parent), d(new Private)
{
    g_type_init();

    AgManager *manager =
        ag_manager_new_for_service_type(serviceType.toUtf8().constData());

    if (manager != 0) {
        d->init(this, manager);
    } else {
        qWarning() << "Manager could not be created, DB is locked";
        d->lastError = Error::DatabaseLocked;
    }

}

/*!
 * Destructor.
 */
Manager::~Manager()
{
    TRACE();

    g_signal_handlers_disconnect_by_func
        (d->m_manager, (void *)&Private::on_enabled_event, this);
    g_signal_handlers_disconnect_by_func
        (d->m_manager, (void *)&Private::on_account_updated, this);
    g_signal_handlers_disconnect_by_func
        (d->m_manager, (void *)&Private::on_account_deleted, this);
    g_signal_handlers_disconnect_by_func
        (d->m_manager, (void *)&Private::on_account_created, this);
    g_object_unref(d->m_manager);

    delete d;
    d = 0;
}

/*!
 * Loads an account from the database.
 * @param id Id of the account to be retrieved.
 *
 * @return Requested account or NULL if not found. If NULL is returned,
 * call lastError() to find out why.
 */
Account *Manager::account(const AccountId &id) const
{
    TRACE() << "get account id: " << id;

    GError *error = NULL;
    AgAccount *account = ag_manager_load_account(d->m_manager, id, &error);

    if (account != NULL) {
        Q_ASSERT(error == NULL);
        Account *tmp = new Account(account, const_cast<Manager*>(this));
        g_object_unref(account);
        return tmp;
    } else {
        Q_ASSERT(error != NULL);
        d->lastError = Error(error);
        g_error_free(error);
    }
    return NULL;
}

/*!
 * Lists the accounts which support the requested service.
 *
 * @param serviceType Type of service that returned accounts must support.
 * If not given and the manager is not constructed with service type,
 * all accounts are returned.
 *
 * @return List of account IDs.
 */
AccountIdList Manager::accountList(const QString &serviceType) const
{
    GList *list = NULL;

    if (serviceType.isEmpty()) {
        list = ag_manager_list(d->m_manager);
    } else {
        list = ag_manager_list_by_service_type(d->m_manager,
            serviceType.toUtf8().constData());
    }

    /* convert glist -> AccountIdList */
    AccountIdList idList;
    GList *iter;

    for (iter = list; iter; iter = g_list_next(iter))
    {
        idList.append((AccountId)GPOINTER_TO_INT(iter->data));
    }

    ag_manager_list_free(list);

    return idList;
}

/*!
 * Lists the enabled accounts which support the requested service that also
 * must be enabled.
 *
 * @param serviceType Type of service that returned accounts must support.
 * If not given and the manager is not constructed with service type,
 * all enabled accounts are returned.
 *
 * @return List of account IDs.
 */
AccountIdList Manager::accountListEnabled(const QString &serviceType) const
{
    GList *list = NULL;
    if (serviceType.isEmpty()) {
        list = ag_manager_list_enabled(d->m_manager);
    } else {
        list = ag_manager_list_enabled_by_service_type(d->m_manager,
            serviceType.toUtf8().constData());
    }

    /* convert glist -> AccountIdList */
    AccountIdList idList;
    GList *iter;

    for (iter = list; iter; iter = g_list_next(iter))
    {
        idList.append((AccountId)GPOINTER_TO_INT(iter->data));
    }

    ag_manager_list_free(list);

    return idList;
}

/*!
 * Creates a new account.
 * @param providerName Name of account provider.
 *
 * @return Created account or NULL if some error occurs.
 */
Account *Manager::createAccount(const QString &providerName)
{
    TRACE() << providerName;

    AgAccount *account =
        ag_manager_create_account(d->m_manager,
                                  providerName.toUtf8().constData());

    if (account != NULL) {
        /* convert gaccount into qaccount */
        Account *tmp = new Account(account, this);
        g_object_unref(account);
        return tmp;
    }

    return NULL;
}

/*!
 * Gets an object representing a service.
 * @param serviceName Name of service to get.
 *
 * @return The requested service or an invalid service if not found.
 */
Service Manager::service(const QString &serviceName) const
{
    TRACE() << serviceName;
    AgService *service =
        ag_manager_get_service(d->m_manager,
                               serviceName.toUtf8().constData());
    return Service(service, StealReference);
}

/*!
 * Gets the service list. If the manager is constructed with given service type
 * only the services which supports the service type will be returned.
 *
 * @param serviceType Type of services to be listed. If not given and
 * the manager is not constructed with service type, all
 * services are listed.
 *
 * @return List of Service objects.
 */
ServiceList Manager::serviceList(const QString &serviceType) const
{
    TRACE() << serviceType;
    GList *list;

    if (serviceType.isEmpty()) {
        list = ag_manager_list_services(d->m_manager);
    } else {
        list = ag_manager_list_services_by_type(d->m_manager,
            serviceType.toUtf8().constData());
    }
    TRACE() << "found:" << g_list_length(list);

    /* convert glist -> ServiceList */
    ServiceList servList;
    GList *iter;

    for (iter = list; iter; iter = g_list_next(iter))
    {
        AgService *service = (AgService*)iter->data;
        servList.append(Service(service, StealReference));
    }

    g_list_free(list);

    return servList;
}

Provider *Manager::providerInstance(AgProvider *provider) const
{
    Provider *ret;

    ret = d->providers.value(provider);
    if (!ret)
    {
        ret = new Provider(provider);
        d->providers.insert(provider, ret);
    }
    return ret;
}

/*!
 * Gets an object representing a provider.
 * @param providerName Name of provider to get.
 *
 * @return Requested provider or NULL if not found.
 */
Provider *Manager::provider(const QString &providerName) const
{
    TRACE() << providerName;
    AgProvider *provider;

    provider = ag_manager_get_provider(d->m_manager,
                                       providerName.toUtf8().constData());
    if (!provider)
        return NULL;

    Provider *prov = providerInstance(provider);
    ag_provider_unref(provider);
    return prov;
}

/*!
 * Gets a provider list.
 *
 * @return List of registered providers.
 */
ProviderList Manager::providerList() const
{
    GList *list;

    list = ag_manager_list_providers(d->m_manager);

    /* convert glist -> ProviderList */
    ProviderList provList;
    GList *iter;

    for (iter = list; iter; iter = g_list_next(iter))
    {
        Provider *prov = providerInstance((AgProvider*)(iter->data));
        provList.append(prov);
    }

    ag_provider_list_free(list);

    return provList;
}

/*!
 * Gets an object representing a service type.
 * @param name Name of service type to load.
 *
 * @return Requested service type or NULL if not found.
 */
ServiceType *Manager::serviceType(const QString &name) const
{
    ServiceType *serviceType = d->serviceTypes.value(name, NULL);
    if (serviceType == 0) {
        AgServiceType *type;
        type = ag_manager_load_service_type(d->m_manager,
                                            name.toUtf8().constData());
        if (type == NULL)
            return NULL;

        serviceType = new ServiceType(type);
        d->serviceTypes.insert(name, serviceType);
        ag_service_type_unref(type);
    }
    return serviceType;
}

/*!
 * Get an object representing an application.
 * @param applicationName Name of the application to load.
 *
 * @return The requested Application, or an invalid Application object if not
 * found.
 */
Application Manager::application(const QString &applicationName) const
{
    QByteArray ba = applicationName.toUtf8();
    AgApplication *application =
        ag_manager_get_application(d->m_manager, ba.constData());
    return Application(application);
}

/*!
 * List the registered applications which support the given service.
 * @param service The service to be supported.
 *
 * @return A list of Application objects.
 */
ApplicationList Manager::applicationList(const Service &service) const
{
    ApplicationList ret;
    GList *applications, *list;

    applications = ag_manager_list_applications_by_service(d->m_manager,
                                                           service.service());
    for (list = applications; list != NULL; list = list->next) {
        AgApplication *application = (AgApplication *)list->data;
        ret.append(Application(application));
    }
    g_list_free (applications);
    return ret;
}

/*!
 * Gets the service type if given in manager constructor.
 *
 * @return Service type or NULL if not given.
 */
QString Manager::serviceType() const
{
    return UTF8(ag_manager_get_service_type (d->m_manager));
}

/*!
 * Sets the timeout for database operations.
 * @param timeout The new timeout in milliseconds.
 *
 * This tells the library how long it is allowed to block while waiting
 * for a locked DB to become accessible. Higher values mean a higher
 * chance of successful reads, but also mean that the execution might be
 * blocked for a longer time. The default is 5 seconds.
 */
void Manager::setTimeout(quint32 timeout)
{
    ag_manager_set_db_timeout(d->m_manager, timeout);
}

/*!
 * Gets the database timeout.
 * @return The timeout (in milliseconds) for database operations.
 */
quint32 Manager::timeout()
{
    return ag_manager_get_db_timeout(d->m_manager);
}

/*!
 * Sets whether to abort the application when a database timeout occurs.
 * By default the library does not abort the application.
 */
void Manager::setAbortOnTimeout(bool abort)
{
    ag_manager_set_abort_on_db_timeout(d->m_manager, abort);
}

/*!
 * @return Whether the application will be aborted when a database timeout
 * occurs.
 */
bool Manager::abortOnTimeout() const
{
    return ag_manager_get_abort_on_db_timeout(d->m_manager);
}

/*!
 * Gets the last error. Not all operations set/reset the error; see the
 * individual methods' documentation to see if they set the last error or
 * not. Call this method right after an account operation
 * has failed; if no error occurred, the result of this method are
 * undefined.
 *
 * @return The last error.
 */
Error Manager::lastError() const
{
    return d->lastError;
}

