/*
 * QObject JSON integration
 *
 * Copyright IBM, Corp. 2009
 *
 * Authors:
 *  Anthony Liguori   <aliguori@us.ibm.com>
 *
 * This work is licensed under the terms of the GNU LGPL, version 2.1 or later.
 * See the COPYING.LIB file in the top-level directory.
 *
 */

#ifndef QJSON_H
#define QJSON_H

QObject *qobject_from_json(const char *string, Error **errp);

QObject *qobject_from_vjsonf_nofail(const char *string, va_list ap)
    GCC_FMT_ATTR(1, 0);
QObject *qobject_from_jsonf_nofail(const char *string, ...)
    GCC_FMT_ATTR(1, 2);
QDict *qdict_from_vjsonf_nofail(const char *string, va_list ap)
    GCC_FMT_ATTR(1, 0);
QDict *qdict_from_jsonf_nofail(const char *string, ...)
    GCC_FMT_ATTR(1, 2);

QString *qobject_to_json(const QObject *obj);
QString *qobject_to_json_pretty(const QObject *obj);

#endif /* QJSON_H */
