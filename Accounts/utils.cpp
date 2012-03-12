/* vi: set et sw=4 ts=4 cino=t0,(0: */
/*
 * This file is part of libaccounts-qt
 *
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

#include "accountscommon.h"
#include "utils.h"

namespace Accounts {

QVariant gvalueToVariant(const GValue *value)
{
    GType type = G_VALUE_TYPE(value);

    QVariant variant;

    switch (type)
    {
    case G_TYPE_STRING:
        variant = UTF8(g_value_get_string(value));
        break;
    case G_TYPE_INT:
        variant = g_value_get_int(value);
        break;
    case G_TYPE_UINT:
        variant = g_value_get_uint(value);
        break;
    case G_TYPE_INT64:
        variant = qint64(g_value_get_int64(value));
        break;
    case G_TYPE_UINT64:
        variant = quint64(g_value_get_uint64(value));
        break;
    case G_TYPE_BOOLEAN:
        variant = bool(g_value_get_boolean(value));
        break;
    default:
        qWarning() << "Unsupported type" << UTF8(G_VALUE_TYPE_NAME(value));
        break;
    }

    return variant;
}

bool variantToGValue(const QVariant &variant, GValue *value)
{
    QByteArray tmpvalue;

    switch (variant.type())
    {
    case QVariant::String:
        g_value_init(value, G_TYPE_STRING);
        tmpvalue = variant.toString().toUtf8();
        g_value_set_string(value, tmpvalue.constData());
        break;
    case QVariant::Int:
        g_value_init(value, G_TYPE_INT);
        g_value_set_int(value, variant.toInt());
        break;
    case QVariant::UInt:
        g_value_init(value, G_TYPE_UINT);
        g_value_set_uint(value, variant.toUInt());
        break;
    case QVariant::LongLong:
        g_value_init(value, G_TYPE_INT64);
        g_value_set_int64(value, variant.toLongLong());
        break;
    case QVariant::ULongLong:
        g_value_init(value, G_TYPE_UINT64);
        g_value_set_uint64(value, variant.toULongLong());
        break;
    case QVariant::Bool:
        g_value_init(value, G_TYPE_BOOLEAN);
        g_value_set_boolean(value, variant.toBool());
        break;
    default:
        qWarning() << "Unsupported datatype" << variant.typeName();
        return false;
    }

    return true;
}

}; // namespace
