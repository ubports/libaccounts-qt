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

#ifndef ACCOUNT_H
#define ACCOUNT_H


#include <QObject>
#include <QSettings>
#include <QStringList>

#include "Accounts/accountscommon.h"
#include "Accounts/service.h"

#define ACCOUNTS_KEY_CREDENTIALS_ID QLatin1String("CredentialsId")

extern "C"
{
    typedef struct _AgAccount AgAccount;
    typedef struct _AgAccountWatch *AgAccountWatch;
}

/*! @namespace Accounts
 *
 */

#define ACCOUNTS_EXPORT

namespace Accounts
{
typedef quint32 AccountId;
typedef QList<AccountId> AccountIdList;
class Manager;

/*!
 * Tells the origin of an account setting: whether it was set on the account,
 * or it's a default value in the service template, or whether it's unset.
 */
enum SettingSource
{
    NONE,
    ACCOUNT,
    TEMPLATE
};

enum ErrorCode
{
    /* The value of this enum must be the same as AgError */
    Database = 0,
    Disposed,
    Deleted,
};

/*!
 * @class Watch
 *
 * @brief Monitors an account key or group of keys.
 *
 * @details A watch is created via the Account::watch method and is a simple
 * object which will emit the notify() signal when the value of the key (or
 * group) that it's monitoring is changed.
 */
class ACCOUNTS_EXPORT Watch : public QObject
{
    Q_OBJECT

public:
    /* We don't want to document these.
     * \cond
     */
    Watch(QObject *parent = 0);
    ~Watch();

    void setWatch(AgAccountWatch w) { watch = w; };
    class Private;
    // \endcond

signals:
    /*!
     * Emitted when the value of the keys monitored by this watch change.
     * @param key The string that was used to create this watch. Note that if
     * this watch is monitor multiple keys this param only identifies their
     * common prefix, and not the actual key being changed.
     */
    void notify(const char *key);

    // \cond
private:
    AgAccountWatch watch;
    friend class Private;
    // \endcond
};

/*! @class Account
 *
 * @brief The Account class provides an interface to account settings.
 *
 * @details The Account class is used to access account and service settings.
 * This class has no constructor, therefore to instantiate one Account object
 * one has to either use the Manager::createAccount method (to create a new
 * empty account) or Manager::account (to load an existing account).
 *
 * Most of the methods in the Account class act on the selected service: for
 * example, calling setEnabled(false) on the NULL service (this is the service
 * to be used for changing settings globally on the account) will disable the
 * account, while the code
 * \code
 * account->selectService(myService);
 * account->setEnabled(false);
 * \endcode
 * will disable the "myService" service.
 *
 * All changes made on an account (including deletion) are not stored until
 * sync() is called.
 */
class ACCOUNTS_EXPORT Account : public QObject
{
    Q_OBJECT

public:

    /*!
     * Destroy current account object and free all resources
     */
    virtual ~Account();

    /*!
     * Returns the AccountId of this account (0 if the account has not yet been
     * stored into the database).
     */
    AccountId id() const;

    /*!
     * Returns the Manager.
     */
    Manager *manager() const;

    /*!
     * Checks whether the account supports the given service
     */
    bool supportsService(const QString &serviceType) const;

    /*!
     * Return a list of services supported by this account.
     *
     * This is currently computed by returning all services having the same
     * provider as the account.
     */
    ServiceList services(const QString &serviceType = NULL) const;

    /*!
     * Checks whether the account is enabled.
     */
    bool enabled() const;

    /*!
     * Enables/disables the account.
     * The change will be written only when sync() is called.
     *
     * This method operates on the currently selected service.
     */
    void setEnabled(bool);

    /*!
     * Get the account's credentials id in signon DB.
     *
     * The credentials ID is first read from the currently selected service;
     * if it's not found, then it is ready from the global account settings.
     * In any case, the currently selected service is not altered.
     */
    qint32 credentialsId();

    /*!
     * set the accounts credentials id
     * The change will be written only when sync() is called.
     *
     * This method operates on the currently selected service.
     */
    void setCredentialsId(const qint32 id) {
        setValue(ACCOUNTS_KEY_CREDENTIALS_ID, id);
    }

    /*!
     * Returns the display name of the account.
     *
     * This method operates on the currently selected service.
     */
    QString displayName() const;

    /*!
     * Changes the display name of the account.
     * The change will be written only when sync() is called.
     */
    void setDisplayName(const QString &displayName);

    /*!
     * Returns the name of the provider of the account.
     */
    QString providerName() const;

    /*!
     * Selects the Service for the subsequent operations.
     * @param service The Service to select. If this is NULL, the global
     * account settings will be selected.
     */
    void selectService(const Service *service = 0);

    /*!
     * Returns the currently selected service.
     */
    Service *selectedService() const;

    /* QSettings-like methods */

    /*!
     * Returns all keys in the current group.
     *
     * This method operates on the currently selected service.
     */
    QStringList allKeys() const;

    /*!
     * Enters a group. This method never fails.
     *
     * This method operates on the currently selected service.
     */
    void beginGroup(const QString &prefix);

    /*!
     * Returns all the groups which are direct children of the current group.
     *
     * This method operates on the currently selected service.
     */
    QStringList childGroups() const;

    /*!
     * Return all the keys which are direct children of the current group.
     *
     * This method operates on the currently selected service.
     */
    QStringList childKeys() const;

    /*!
     * Removes all the keys in the currently selected service.
     * @see remove(const QString &key)
     */
    void clear();

    /*!
     * Check whether the given key is in the current group.
     *
     * This method operates on the currently selected service.
     */
    bool contains(const QString &key) const;

    /*!
     * Exits a group.
     *
     * This method operates on the currently selected service.
     */
    void endGroup();

    /*!
     * Returns the name of the current group.
     *
     * This method operates on the currently selected service.
     */
    QString group() const;

    /*!
     * Checks whether the account is writable. This always returns true.
     */
    bool isWritable() const;

    /*!
     * Remove the given key. If the key is the empty string, all keys in the
     * current group are removed.
     *
     * This method operates on the currently selected service.
     */
    void remove(const QString &key);

    /*!
     * Changes the value of an account setting.
     * @param key The key name of the setting.
     * @param value The new value.
     *
     * This method operates on the currently selected service.
     */
    void setValue(const QString &key, const QVariant &value);

    /*!
     * Stores all account settings into the database.
     * The signal synced() will be emitted in case of success, or
     * error() in case of failure. No assumption must be made about when these
     * signals will be emitted: if the database is locked, the signals might
     * be emitted asynchronously, whereas if the operation can happen
     * synchronously then the signals can be emitted before this method
     * returns.
     * If for some reason one would want to process the signals asynchronously
     * from the event loop, one can use the Qt::QueuedConnection connection
     * type as last parameter of the QObject::connect call.
     */
    void sync();

    /*!
     * Blocking version of the sync() method: execution of the current thread
     * will block until the operation has completed.
     * Usage of this method is discouraged, especially for UI applications.
     *
     * @return true on success, false otherwise.
     */
    bool syncAndBlock();

    /*!
     * Retrieves the value of an account setting, as a QVariant.
     * @param key The key whose value must be retrieved.
     * @param value A QVariant initialized to the expected type of the value.
     * @see valueAsString
     * @see valueAsInt
     * @see valueAsBool
     *
     * @return Whether the value comes from the account, the service template
     * or was unset.
     *
     * This method operates on the currently selected service.
     */
    SettingSource value(const QString &key, QVariant &value) const;

    /*!
     * Gets an account setting as a string.
     * @param key The key whose value must be retrieved.
     * @param default_value Value returned if the key is unset.
     * @param source Indicates whether the value comes from the account, the
     * service template or was unset.
     *
     * This method operates on the currently selected service.
     */
    QString valueAsString(const QString &key,
                          QString default_value = 0,
                          SettingSource *source = 0) const;

    /*!
     * Gets an account setting as an integer.
     * @param key The key whose value must be retrieved.
     * @param default_value Value returned if the key is unset.
     * @param source Indicates whether the value comes from the account, the
     * service template or was unset.
     *
     * This method operates on the currently selected service.
     */
    int valueAsInt(const QString &key,
                   int default_value = 0,
                   SettingSource *source = 0) const;

    /*!
     * Gets an account setting as an usigned long long integer.
     * @param key The key whose value must be retrieved.
     * @param default_value Value returned if the key is unset.
     * @param source Indicates whether the value comes from the account, the
     * service template or was unset.
     *
     * This method operates on the currently selected service.
     */
    quint64 valueAsUInt64(const QString &key,
                   quint64 default_value = 0,
                   SettingSource *source = 0) const;

    /*!
     * Gets an account setting as a boolean.
     * @param key The key whose value must be retrieved.
     * @param default_value Value returned if the key is unset.
     * @param source Indicates whether the value comes from the account, the
     * service template or was unset.
     *
     * This method operates on the currently selected service.
     */
    bool valueAsBool(const QString &key,
                     bool default_value = false,
                     SettingSource *source = 0) const;
    /*!
     * Install a key/group watch.
     *
     * @param key The key to watch; if %NULL, watches the currently selected
     * group.
     *
     * @return A Watch object.
     *
     * This method operates on the currently selected service.
     */
    Watch *watchKey(const QString &key = NULL);

    /*!
     * Marks the account for removal.
     * The account will be deleted only when the sync() method is called.
     */
    void remove();

    /*!
     * Creates signature of key with given token.
     * @param key The key or the prefix of set of the keys to be signed.
     * @param token The token to be used for signing the key.
     *
     * This method operates on the currently selected service.
     */
    void sign(const QString &key, const char *token);

    /*!
     * Verify if the key is signed and the signature matches the value
     * and provides the token which was used for signing the key.
     *
     * @param key The name of the key or prefix of the keys to be verified.
     * @param token Token to be retrieved.
     *
     * @return true if the key is signed and the signature matches the value.
     *
     * This method operates on the currently selected service.
     */
    bool verify(const QString &key, const char **token);

    /*!
     * Verify if the key is signed with any of the tokens and the signature
     * is valid.
     *
     * @param key The name of the key or prefix of the keys to be verified.
     * @param tokens Array of the tokens.
     *
     * @return true if the key is signed with any of the token and the signature
     * is valid.
     *
     * This method operates on the currently selected service.
     */
    bool verifyWithTokens(const QString &key, QList<const char*> tokens);

signals:
    void displayNameChanged(const QString &displayName);
    void enabledChanged(const QString &serviceName, bool enabled);

    void error(Accounts::ErrorCode errorCode);
    void synced();

    void removed();

protected:
    // Don't include constructor in docs: \cond
    Account(AgAccount *account, QObject *parent = 0);
    // \endcond

private:
    // Don't include private data in docs: \cond
    class Private;
    friend class Manager;
    friend class Account::Private;
    friend class Watch;

    Private *d;
    // \endcond
};


} //namespace Accounts

#endif // ACCOUNT_H
