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

#include <QCoreApplication>
#include <QDebug>
#include "account-tool.h"

using namespace Accounts;

int main(int argc, char **argv)
{
    int ret = 0;
    QCoreApplication app(argc, argv);

    QStringList args = QCoreApplication::arguments ();
    QString cmd;
    QString param;
    QString type;

    if(args.size()<=1)
    {
        qDebug("account-tool");
        qDebug("Usage: account-tool [-t type] [options]");
        qDebug("  -l       list accounts");
        qDebug("  -L       list account names");
        qDebug("  -k [#]   list keys for account #");
        qDebug("  -t type  list accounts with type type");

    }
    for (int i = 0; i < args.size(); ++i)
    {
        //qDebug ( args.at(i).toLocal8Bit().constData());

        if(args.at(i).startsWith("-"))
        {
            cmd = args.at(i).mid(1);
            if(args.size()>i+1)
            {
                if(cmd == QString("t"))
                {
                    type =args.at(i+1);
                }
                else
                param = args.at(i+1);
            }
        //qDebug ("command: %s",cmd.toLocal8Bit().constData());
        }
    }

    Manager* accountMgr = new Manager();

    const AccountIdList acclist=accountMgr->accountList(type);

    if (cmd == QString("l")) {
        qDebug("list accounts:");
        for (int i = 0; i < acclist.size(); ++i)
              qDebug ( "%u", acclist.at(i));
    }

    if (cmd == QString("L")) {
        qDebug("List accounts:");
        for (int i = 0; i < acclist.size(); ++i)
        {
            qDebug ( "Account: %u", acclist.at(i));
            Account* acc = accountMgr->account(acclist.at(i));
            if (acc!=NULL)
            {
            qDebug ( "%s", acc->displayName().toLocal8Bit().constData());
            }
         // qDebug ( acc->displayName().toLocal8Bit().constData());
        acc=NULL;
        }
    }

    if (cmd == QString("k")) {
        qDebug("List keys:");
        for (int i = 0; i < acclist.size(); ++i)
        {
            if(param.isEmpty() || param.toInt()==int(acclist.at(i)))
            {
                qDebug ( "Account: %u", acclist.at(i));
                Account* acc = accountMgr->account(acclist.at(i));
                if (acc!=NULL)
                {
                    qDebug ( "Display name: %s", acc->displayName().toLocal8Bit().constData());
                    qDebug ( "CredentialsId: %d", acc->credentialsId());
                    qDebug ( "Provider: %s", acc->providerName().toLocal8Bit().constData());

                    const QStringList keylist=acc->allKeys();
                    for (int i = 0; i < keylist.size(); ++i) {
                        //QVariant val;
                        //acc->value(keylist.at(i), val);
                        qDebug() << keylist.at(i).toLocal8Bit().constData() << " = " << acc->valueAsString(keylist.at(i));
                        }
                }
                acc=NULL;
            }
        }
    }
/*
    Profile p=Profile(QString("test"));
    p.setValue(QString("name"),QString("test_value"));
    const QStringList plist=p.allKeys();
    for (int i = 0; i < plist.size(); ++i)
          qDebug ( plist.at(i).toLocal8Bit().constData());
*/


// use this when doing something async
//    QObject::connect(&hello, SIGNAL(clicked()), &app, SLOT(quit()));
//    ret = app.exec();

    delete accountMgr;

    return ret;
}

