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

#include <QtCore>

#include "service.h"
#include "manager.h"

#include <libaccounts-glib/ag-manager.h>
#include <libaccounts-glib/ag-account.h>


namespace Accounts {

class Manager::Private
{
    typedef QHash<AgProvider *, Provider *> ProviderHash;
    typedef QHash<AgService *, Service *> ServiceHash;
public:
    Private()
    {
        providers = ProviderHash();
        services = ServiceHash();
        m_manager = 0;
    }

    ~Private() {
        foreach (Provider *provider, providers)
        {
            delete provider;
        }
        providers.clear();
        foreach (Service *service, services)
        {
            delete service;
        }
        services.clear();
    }

    AgManager *m_manager; //real manager
    ProviderHash providers;
    ServiceHash services;

    static void on_account_created(Manager *self, AgAccountId id);
    static void on_account_deleted(Manager *self, AgAccountId id);
};

} //namespace Accounts

using namespace Accounts;

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

Manager::Manager(QObject *parent)
    : QObject(parent), d(new Private)
{
    g_type_init();
    d->m_manager = ag_manager_new();

    g_signal_connect_swapped
        (d->m_manager, "account-created",
         G_CALLBACK(&Private::on_account_created), this);
    g_signal_connect_swapped
        (d->m_manager, "account-deleted",
         G_CALLBACK(&Private::on_account_deleted), this);
}

Manager::~Manager()
{
    TRACE();

    g_signal_handlers_disconnect_by_func
        (d->m_manager, (void *)&Private::on_account_created, this);
    g_signal_handlers_disconnect_by_func
        (d->m_manager, (void *)&Private::on_account_deleted, this);
    g_object_unref(d->m_manager);

    delete d;
    d = 0;
}

Account* Manager::account(const AccountId &id) const
{
    TRACE() << "get account id: " << id;

    AgAccount *account = ag_manager_get_account(d->m_manager, id);

    if(account != NULL) {
        Account* tmp = new Account(account, const_cast<Manager*>(this));
        g_object_unref(account);
        return tmp;
    }
    return NULL;
}

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
        idList.append((AccountId)(iter->data));
    }

    ag_manager_list_free(list);

    return idList;
}

Account* Manager::createAccount(const QString &providerName)
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

Service *Manager::serviceInstance(AgService *service) const
{
    Service *ret;

    ret = d->services.value(service);
    if (!ret)
    {
        ret = new Service(service);
        d->services.insert(service, ret);
    }
    return ret;
}

Service* Manager::service(const QString &serviceName) const
{
    TRACE() << serviceName;
    AgService* service =
        ag_manager_get_service(d->m_manager,
                               serviceName.toUtf8().constData());
    if (!service)
        return NULL;

    Service *serv= serviceInstance(service);
    ag_service_unref(service);
    return serv;
}

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
        Service* serv = serviceInstance((AgService*)(iter->data));
        servList.append(serv);
    }

    ag_service_list_free(list);

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

Provider* Manager::provider(const QString &providerName) const
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