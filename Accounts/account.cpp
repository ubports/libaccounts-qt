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

#include "account.h"
#include "manager.h"

#include <libaccounts-glib/ag-manager.h>
#include <libaccounts-glib/ag-account.h>

namespace Accounts {

class Account::Private
{
public:
    Private()
    {
        m_account = 0;
    }

    ~Private() {}

    AgAccount *m_account;  //real account
    QString prefix;

    static void on_display_name_changed(Account *self);
    static void on_enabled(Account *self, const gchar *service_name,
                           gboolean enabled);
    static void account_store_cb(AgAccount *account, const GError *error,
                                 Account *self);
    static void on_deleted(Account *self);
};

class Watch::Private
{
public:
    static void account_notify_cb(AgAccount *account, const gchar *key,
                                  Watch *self);
};
} //namespace Accounts


using namespace Accounts;

static QChar slash = QChar::fromLatin1('/');

Watch::Watch(QObject *parent)
    : QObject(parent)
{
}

Watch::~Watch()
{
    TRACE();
    Account *account = qobject_cast<Account *>(QObject::parent());
    /* The destructor of Account deletes the child Watches before detaching
     * them, so here account should always be not NULL */
    Q_ASSERT(account != NULL);
    ag_account_remove_watch(account->d->m_account, watch);
}

void Account::Private::on_display_name_changed(Account *self)
{
    TRACE();
    const gchar *name = ag_account_get_display_name(self->d->m_account);

    emit self->displayNameChanged(UTF8(name));
}

void Account::Private::on_enabled(Account *self, const gchar *service_name,
                                  gboolean enabled)
{
    TRACE();

    emit self->enabledChanged(UTF8(service_name), enabled);
}

void Account::Private::on_deleted(Account *self)
{
    TRACE();

    emit self->removed();
}

Account::Account(AgAccount *account, QObject *parent)
    : QObject(parent), d(new Private)
{
    TRACE();
    d->m_account = account;
    g_object_ref(account);

    g_signal_connect_swapped(account, "display-name-changed",
                             G_CALLBACK(&Private::on_display_name_changed),
                             this);
    g_signal_connect_swapped(account, "enabled",
                             G_CALLBACK(&Private::on_enabled), this);
    g_signal_connect_swapped(account, "deleted",
                             G_CALLBACK(&Private::on_deleted), this);
}

Account::~Account()
{
    TRACE();

    QObjectList list = children();
    for (int i = 0; i < list.count(); i++)
    {
        QObject *o = list.at(i);
        if (qobject_cast<Watch *>(o))
            delete o;
    }

    g_signal_handlers_disconnect_by_func
        (d->m_account, (void *)&Private::on_display_name_changed, this);
    g_signal_handlers_disconnect_by_func
        (d->m_account, (void *)&Private::on_enabled, this);
    g_signal_handlers_disconnect_by_func
        (d->m_account, (void *)&Private::on_deleted, this);
    g_object_unref(d->m_account);
    delete d;
    d = 0;
}

AccountId Account::id() const
{
    return d->m_account ? d->m_account->id : 0;
}

Manager *Account::manager() const
{
    return qobject_cast<Manager *>(QObject::parent());
}

bool Account::supportsService(const QString &serviceType) const
{
    TRACE() << serviceType;

    return ag_account_supports_service(d->m_account,
                                       serviceType.toUtf8().constData());
}

ServiceList Account::services(const QString &serviceType) const
{
    TRACE() << serviceType;

    GList *list;
    if (serviceType.isEmpty()) {
        list = ag_account_list_services(d->m_account);
    } else {
        list = ag_account_list_services_by_type(d->m_account,
            serviceType.toUtf8().constData());
    }

    /* convert glist -> ServiceList */
    ServiceList servList;
    GList *iter;
    Manager *mgr = manager();
    Q_ASSERT(mgr != 0);
    for (iter = list; iter; iter = g_list_next(iter))
    {
        Service *serv = mgr->serviceInstance((AgService*)(iter->data));
        servList.append(serv);
    }

    ag_service_list_free(list);

    return servList;
}

ServiceList Account::enabledServices() const
{
    GList *list;
    list = ag_account_list_enabled_services(d->m_account);

    /* convert glist -> ServiceList */
    ServiceList servList;
    GList *iter;
    Manager *mgr = manager();
    Q_ASSERT(mgr != 0);
    for (iter = list; iter; iter = g_list_next(iter))
    {
        Service *serv = mgr->serviceInstance((AgService*)(iter->data));
        servList.append(serv);
    }

    ag_service_list_free(list);

    return servList;
}

bool Account::enabled() const
{
    return ag_account_get_enabled(d->m_account);
}

void Account::setEnabled(bool enabled)
{
    ag_account_set_enabled(d->m_account, enabled);
}

QString Account::displayName() const
{
    return UTF8(ag_account_get_display_name(d->m_account));
}

void Account::setDisplayName(const QString &displayName)
{
    ag_account_set_display_name(d->m_account,
                                displayName.toUtf8().constData());
}

QString Account::providerName() const
{
    return UTF8(ag_account_get_provider_name(d->m_account));
}

void Account::selectService(const Service *service)
{
    AgService *agService = NULL;

    if (service != NULL)
        agService = service->service();

    ag_account_select_service(d->m_account, agService);
    d->prefix = QString();
}

Service* Account::selectedService() const
{
    AgService *agService = ag_account_get_selected_service(d->m_account);
    if (agService == NULL)
        return NULL;

    Manager *mgr = manager();
    Q_ASSERT(mgr != 0);
    Service *service = mgr->serviceInstance(agService);

    return service;
}

QStringList Account::allKeys() const
{
    QStringList allKeys;
    AgAccountSettingIter iter;
    const gchar *key;
    const GValue *val;

    /* iterate the settings */
    QByteArray tmp = d->prefix.toLatin1();
    ag_account_settings_iter_init(d->m_account, &iter, tmp.constData());
    while (ag_account_settings_iter_next(&iter, &key, &val))
    {
        allKeys.append(QString(ASCII(key)).mid(d->prefix.size()));
    }
    return allKeys;
}

void Account::beginGroup(const QString &prefix)
{
    d->prefix += prefix + slash;
}

QStringList Account::childGroups() const
{
    QStringList groups, all_keys;

    all_keys = allKeys();
    foreach (QString key, all_keys)
    {
        if (key.contains(slash)) {
            QString group = key.section(slash, 0, 0);
            if (!groups.contains(group))
                groups.append(group);
        }
    }
    return groups;
}

QStringList Account::childKeys() const
{
    QStringList keys, all_keys;

    all_keys = allKeys();
    foreach (QString key, all_keys)
    {
        if (!key.contains(slash))
            keys.append(key);
    }
    return keys;
}

void Account::clear()
{
    /* clear() must ignore the group: so, temporarily reset it and call
     * remove("") */
    QString saved_prefix = d->prefix;
    d->prefix = QString();
    remove(QString());
    d->prefix = saved_prefix;
}

bool Account::contains(const QString &key) const
{
    return childKeys().contains(key);
}

void Account::endGroup()
{
    d->prefix = d->prefix.section(slash, 0, -3,
                                  QString::SectionIncludeTrailingSep);
    if (d->prefix[0] == slash) d->prefix.remove(0, 1);
}

QString Account::group() const
{
    if (d->prefix.endsWith(slash))
        return d->prefix.left(d->prefix.size() - 1);
    return d->prefix;
}

bool Account::isWritable() const
{
    return true;
}

void Account::remove(const QString &key)
{
    if (key.isEmpty())
    {
        /* delete all keys in the group */
        QStringList keys = allKeys();
        foreach (QString key, keys)
        {
            if (!key.isEmpty())
                remove(key);
        }
    }
    else
    {
        QString full_key = d->prefix + key;
        QByteArray tmpkey = full_key.toLatin1();
        ag_account_set_value(d->m_account, tmpkey.constData(), NULL);
    }
}

void Account::setValue(const QString &key, const QVariant &value)
{
    TRACE();
    GValue val= {0, {{0}}};
    QByteArray tmpvalue;

    switch (value.type())
    {
    case QVariant::String:
        g_value_init(&val, G_TYPE_STRING);
        tmpvalue = value.toString().toUtf8();
        g_value_set_static_string(&val, tmpvalue.constData());
        break;
    case QVariant::Int:
        g_value_init(&val, G_TYPE_INT);
        g_value_set_int(&val, value.toInt());
        break;
    case QVariant::UInt:
        g_value_init(&val, G_TYPE_UINT);
        g_value_set_uint(&val, value.toUInt());
        break;
    case QVariant::LongLong:
        g_value_init(&val, G_TYPE_INT64);
        g_value_set_int64(&val, value.toLongLong());
        break;
    case QVariant::ULongLong:
        g_value_init(&val, G_TYPE_UINT64);
        g_value_set_uint64(&val, value.toULongLong());
        break;
    case QVariant::Bool:
        g_value_init(&val, G_TYPE_BOOLEAN);
        g_value_set_boolean(&val, value.toBool());
        break;
    default:
        qWarning("unsupproted datatype %s", value.typeName());
        return;
    }

    QString full_key = d->prefix + key;
    QByteArray tmpkey = full_key.toLatin1();
    ag_account_set_value(d->m_account, tmpkey.constData(), &val);
    g_value_unset(&val);
}

void Account::Private::account_store_cb(AgAccount *account, const GError *err,
                                        Account *self)
{
    TRACE() << "Saved accunt ID:" << account->id;

    if (err) {
        emit self->error(Error(err));
        // TODO: remove the next line at some point
        emit self->error((ErrorCode)err->code);
    } else {
        emit self->synced();
    }

    Q_UNUSED(account);
}

void Account::sync()
{
    TRACE();

    ag_account_store(d->m_account,
                     (AgAccountStoreCb)&Private::account_store_cb,
                     this);
}

bool Account::syncAndBlock()
{
    TRACE();

    GError *error = NULL;
    bool ret;

    ret = ag_account_store_blocking(d->m_account, &error);
    if (error)
    {
        qWarning() << "Store operation failed: " << error->message;
        g_error_free(error);
    }

    return ret;
}

SettingSource Account::value(const QString &key, QVariant &value) const
{
    GType type;

    switch (value.type())
    {
    case QVariant::String:
        type = G_TYPE_STRING;
        break;
    case QVariant::Int:
        type = G_TYPE_INT;
        break;
    case QVariant::UInt:
        type = G_TYPE_UINT;
        break;
    case QVariant::LongLong:
        type = G_TYPE_INT64;
        break;
    case QVariant::ULongLong:
        type = G_TYPE_UINT64;
        break;
    case QVariant::Bool:
        type = G_TYPE_BOOLEAN;
        break;
    default:
        qWarning("Unsupported type %s", value.typeName());
        return NONE;
    }

    GValue val= {0, {{0}}};
    g_value_init(&val, type);
    QString full_key = d->prefix + key;
    AgSettingSource source =
        ag_account_get_value(d->m_account,
                             full_key.toLatin1().constData(), &val);
    if (source == AG_SETTING_SOURCE_NONE)
        return NONE;

    switch (type)
    {
    case G_TYPE_STRING:
        value = UTF8(g_value_get_string(&val));
        break;
    case G_TYPE_INT:
        value = g_value_get_int(&val);
        break;
    case G_TYPE_UINT:
        value = g_value_get_uint(&val);
        break;
    case G_TYPE_INT64:
        value = g_value_get_int64(&val);
        break;
    case G_TYPE_UINT64:
        value = g_value_get_uint64(&val);
        break;
    case G_TYPE_BOOLEAN:
        value = g_value_get_boolean(&val);
        break;
    default:
        /* This can never be reached */
        Q_ASSERT(false);
        break;
    }
    g_value_unset(&val);

    return (source == AG_SETTING_SOURCE_ACCOUNT) ? ACCOUNT : TEMPLATE;
}

QString Account::valueAsString(const QString &key,
                               QString default_value,
                               SettingSource *source) const
{
    QVariant var = default_value;
    SettingSource src = value(key, var);
    if (source)
        *source = src;
    return var.toString();
}

int Account::valueAsInt(const QString &key,
                        int default_value,
                        SettingSource *source) const
{
    QVariant var = default_value;
    SettingSource src = value(key, var);
    if (source)
        *source = src;
    return var.toInt();
}

quint64 Account::valueAsUInt64(const QString &key,
                        quint64 default_value,
                        SettingSource *source) const
{
    QVariant var = default_value;
    SettingSource src = value(key, var);
    if (source)
        *source = src;
    return var.toULongLong();
}

bool Account::valueAsBool(const QString &key,
                          bool default_value,
                          SettingSource *source) const
{
    QVariant var = default_value;
    SettingSource src = value(key, var);
    if (source)
        *source = src;
    return var.toBool();
}

void Watch::Private::account_notify_cb(AgAccount *account, const gchar *key,
                                       Watch *watch)
{
    emit watch->notify(key);

    Q_UNUSED(account);
}

Watch *Account::watchKey(const QString &key)
{
    AgAccountWatch ag_watch;
    Watch *watch = new Watch(this);

    if (!key.isEmpty())
    {
        QString full_key = d->prefix + key;
        ag_watch = ag_account_watch_key
            (d->m_account, full_key.toLatin1().constData(),
             (AgAccountNotifyCb)&Watch::Private::account_notify_cb, watch);
    }
    else
    {
        ag_watch = ag_account_watch_dir
            (d->m_account, d->prefix.toLatin1().constData(),
             (AgAccountNotifyCb)&Watch::Private::account_notify_cb, watch);
    }

    if (!ag_watch)
    {
        delete watch;
        return NULL;
    }

    watch->setWatch(ag_watch);
    return watch;
}

void Account::remove()
{
    TRACE();
    ag_account_delete(d->m_account);
}

void Account::sign(const QString &key, const char *token)
{
    ag_account_sign (d->m_account, key.toUtf8().constData(), token);
}

bool Account::verify(const QString &key, const char **token)
{
    return ag_account_verify(d->m_account, key.toUtf8().constData(), token);
}

bool Account::verifyWithTokens(const QString &key, QList<const char*> tokens)
{
    int tokensCount = tokens.count();

    const char *tmp[tokensCount + 1];

    for (int i = 0; i < tokensCount; ++i)
    {
        tmp[i] = tokens.at(i);
    }
    tmp[tokensCount] = NULL;

    return ag_account_verify_with_tokens(d->m_account, key.toUtf8().constData(), tmp);
}

qint32 Account::credentialsId()
{
    QString key = ACCOUNTS_KEY_CREDENTIALS_ID;
    QVariant val(QVariant::Int);

    if (value(key, val) != NONE)
        return val.toInt();

    qint32 id = 0;
    Service *service = selectedService();
    if (service) {
        selectService(NULL);
        if (value(key, val) != NONE)
            id = val.toInt();
        selectService(service);
    }
    return id;
}
