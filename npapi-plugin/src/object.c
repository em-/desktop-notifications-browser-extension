/*
 * This file is part of the desktop-notifications-browser-plugin.
 * Copyright (C) Canonical Ltd. 2012
 * Copyright (C) Collabora Ltd. 2013
 *
 * Author:
 *   Emanuele Aina <emanuele.aina@collabora.com>
 *
 * Based on webaccounts-browser-plugin by:
 *   Alberto Mardegan <alberto.mardegan@canonical.com>
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
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "object.h"

#include <glib.h>
#include <libnotify/notify.h>

typedef struct {
    NPObject object;
    GHashTable *methods;
} DnObjectWrapper;

typedef NPVariant (*DnMethod) (NPObject *object,
                               const NPVariant *args,
                               uint32_t argc);

static gchar *variant_to_string(const NPVariant *variant)
{
    return g_strndup (NPVARIANT_TO_STRING (*variant).UTF8Characters,
                      NPVARIANT_TO_STRING (*variant).UTF8Length);
}

static NPObject *
NPClass_Allocate (NPP instance, NPClass *aClass)
{
    g_return_val_if_fail (instance != NULL, NULL);

    DnObjectWrapper *wrapper = g_new0 (DnObjectWrapper, 1);

    wrapper->methods = g_hash_table_new (g_str_hash, g_str_equal);

    return (NPObject *)wrapper;
}


static void
NPClass_Deallocate (NPObject *npobj)
{
    DnObjectWrapper *wrapper = (DnObjectWrapper *) npobj;

    g_return_if_fail (wrapper != NULL);

    g_hash_table_unref (wrapper->methods);
    g_free (wrapper);
}

static void
NPClass_Invalidate (NPObject *npobj)
{
}

static bool
NPClass_HasMethod (NPObject *npobj, NPIdentifier name)
{
    DnObjectWrapper *wrapper = (DnObjectWrapper *) npobj;
    gchar *method_name;
    gboolean has_method;

    g_return_val_if_fail (wrapper != NULL, false);

    method_name = NPN_UTF8FromIdentifier (name);
    has_method = (g_hash_table_lookup (wrapper->methods, method_name) != NULL);

    g_debug ("%s(\"%s\")", G_STRFUNC, method_name);

    NPN_MemFree (method_name);
    return has_method;
}


static bool
NPClass_Invoke (NPObject *npobj, NPIdentifier name,
                const NPVariant *args, uint32_t argc, NPVariant *result)
{
    DnObjectWrapper *wrapper = (DnObjectWrapper *) npobj;
    gchar *method_name;
    DnMethod method;

    g_return_val_if_fail (wrapper != NULL, false);

    method_name = NPN_UTF8FromIdentifier (name);
    method = g_hash_table_lookup (wrapper->methods, method_name);

    g_debug ("%s(\"%s\")", G_STRFUNC, method_name);

    NPN_MemFree (method_name);

    if (G_UNLIKELY (method == NULL))
        return false;

    *result = method (npobj, args, argc);
    return true;
}

static bool
NPClass_InvokeDefault (NPObject *npobj, const NPVariant *args, uint32_t argc,
                       NPVariant *result)
{
    return false;
}

static bool
NPClass_HasProperty (NPObject *npobj, NPIdentifier name)
{
    return false;
}

static bool
NPClass_GetProperty (NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    return false;
}


static bool
NPClass_SetProperty (NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
    return false;
}


static bool
NPClass_RemoveProperty (NPObject *npobj, NPIdentifier name)
{
    return false;
}


static bool
NPClass_Enumerate (NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
    return false;
}


static bool
NPClass_Construct (NPObject *npobj, const NPVariant *args, uint32_t argc,
                   NPVariant *result)
{
    return false;
}

static NPClass js_object_class = {
    .structVersion = NP_CLASS_STRUCT_VERSION,
    .allocate = NPClass_Allocate,
    .deallocate = NPClass_Deallocate,
    .invalidate = NPClass_Invalidate,
    .hasMethod = NPClass_HasMethod,
    .invoke = NPClass_Invoke,
    .invokeDefault = NPClass_InvokeDefault,
    .hasProperty = NPClass_HasProperty,
    .getProperty = NPClass_GetProperty,
    .setProperty = NPClass_SetProperty,
    .removeProperty = NPClass_RemoveProperty,
    .enumerate = NPClass_Enumerate,
    .construct = NPClass_Construct
};

static NPVariant
dn_show_notification (NPObject *object,
                      const NPVariant *args,
                      uint32_t argc)
{
    NPVariant result;
    gchar *summary, *body = NULL;
    gboolean shown;

    BOOLEAN_TO_NPVARIANT (FALSE, result);

    g_debug ("%s()", G_STRFUNC);

    if (G_UNLIKELY (argc < 1 || !NPVARIANT_IS_STRING(args[0])))
        return result;

    summary = variant_to_string (&args[0]);
    if (G_UNLIKELY (summary == NULL))
        return result;

    if (argc >= 2)
        body = variant_to_string (&args[1]);

    g_debug ("%s(\"%s\", \"%s\", ...)", G_STRFUNC, summary, body);

    NotifyNotification *notification = notify_notification_new (summary, body, NULL);
    shown = notify_notification_show (notification, NULL);

    /*TODO: notify_notification_set_image_from_pixbuf() */

    BOOLEAN_TO_NPVARIANT (shown, result);

    g_free (summary);
    if (body != NULL)
        g_free (body);
    return result;
}

NPObject *
dn_create_plugin_object (NPP instance)
{
    NPObject *object = NPN_CreateObject (instance, &js_object_class);
    g_return_val_if_fail (object != NULL, NULL);

    g_debug ("%s()", G_STRFUNC);

    DnObjectWrapper *wrapper =
        (DnObjectWrapper *) object;

    g_hash_table_insert (wrapper->methods, "showNotification", dn_show_notification);

    return object;
}
