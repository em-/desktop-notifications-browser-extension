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

#include "plugin.h"
#include "object.h"

#include <glib.h>
#include <libnotify/notify.h>

#define PLUGIN_NAME        "Desktop notifications plugin"
#define PLUGIN_DESCRIPTION "Native desktop notifications for Linux desktop environments"
#define PLUGIN_VERSION     PACKAGE_VERSION

typedef struct {
    NPPluginFuncs *plugin_funcs;
    NPP instance;
} DnBrowserPlugin;

static NPNetscapeFuncs *browser_funcs = NULL;

NP_EXPORT(NPError)
NP_Initialize(NPNetscapeFuncs *bFuncs, NPPluginFuncs *pFuncs)
{
    g_debug ("%s()", G_STRFUNC);
    browser_funcs = bFuncs;

    g_warn_if_fail (notify_init (PLUGIN_NAME));

    // Check the size of the provided structure based on the offset of the
    // last member we need.
    if (pFuncs->size < (offsetof(NPPluginFuncs, setvalue) + sizeof(void*)))
        return NPERR_INVALID_FUNCTABLE_ERROR;

    pFuncs->newp = NPP_New;
    pFuncs->destroy = NPP_Destroy;
    pFuncs->setwindow = NPP_SetWindow;
    pFuncs->newstream = NPP_NewStream;
    pFuncs->destroystream = NPP_DestroyStream;
    pFuncs->asfile = NPP_StreamAsFile;
    pFuncs->writeready = NPP_WriteReady;
    pFuncs->write = NPP_Write;
    pFuncs->print = NPP_Print;
    pFuncs->event = NPP_HandleEvent;
    pFuncs->urlnotify = NPP_URLNotify;
    pFuncs->getvalue = NPP_GetValue;
    pFuncs->setvalue = NPP_SetValue;

    return NPERR_NO_ERROR;
}

NP_EXPORT(char*)
NP_GetPluginVersion()
{
    return PLUGIN_VERSION;
}

NP_EXPORT(const char*)
NP_GetMIMEDescription()
{
    g_debug ("%s()", G_STRFUNC);
    return "application/x-desktop-notifications:none:" PLUGIN_NAME;
}

NP_EXPORT(NPError)
NP_GetValue(void *future, NPPVariable aVariable, void *aValue)
{
    switch (aVariable) {
    case NPPVpluginNameString:
        *((char**)aValue) = PLUGIN_NAME;
        break;
    case NPPVpluginDescriptionString:
        *((char**)aValue) = PLUGIN_DESCRIPTION;
        break;
    default:
        return NPERR_INVALID_PARAM;
        break;
    }
    return NPERR_NO_ERROR;
}

NP_EXPORT(NPError)
NP_Shutdown()
{
    notify_uninit();
    return NPERR_NO_ERROR;
}

NPError
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode,
        int16_t argc, char *argn[], char *argv[], NPSavedData *saved)
{
    if (G_UNLIKELY (instance == NULL))
        return NPERR_INVALID_INSTANCE_ERROR;

    DnBrowserPlugin *plugin = g_new0 (DnBrowserPlugin, 1);
    plugin->instance = instance;
    instance->pdata = plugin;
    return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData **save)
{
    g_debug ("%s()", G_STRFUNC);

    if (G_UNLIKELY (instance == NULL || instance->pdata == NULL))
        return NPERR_NO_ERROR;

    DnBrowserPlugin *plugin = instance->pdata;
    g_free (plugin);

    return NPERR_NO_ERROR;
}

NPError
NPP_SetWindow(NPP instance, NPWindow *window)
{
    return NPERR_NO_ERROR;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType type, NPStream *stream, NPBool seekable,
              uint16_t *stype)
{
    return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream *stream, NPReason reason)
{
    return NPERR_GENERIC_ERROR;
}

int32_t
NPP_WriteReady(NPP instance, NPStream *stream)
{
    return 0;
}

int32_t
NPP_Write(NPP instance, NPStream *stream, int32_t offset, int32_t len,
          void *buffer)
{
    return 0;
}

void
NPP_StreamAsFile(NPP instance, NPStream *stream, const char *fname)
{
}

void
NPP_Print(NPP instance, NPPrint *platformPrint)
{
}

int16_t
NPP_HandleEvent(NPP instance, void *event)
{
    return 0;
}

void
NPP_URLNotify(NPP instance, const char *URL, NPReason reason, void *notifyData) {

}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    NPBool support;

    g_debug ("%s()", G_STRFUNC);

    if (G_UNLIKELY (instance == NULL || instance->pdata == NULL))
        return NPERR_INVALID_INSTANCE_ERROR;

    switch (variable) {
    case NPPVpluginScriptableNPObject:
        *(NPObject **)value = dn_create_plugin_object (instance);
        break;
    case NPPVpluginNeedsXEmbed:
        support = false;
        NPN_GetValue (instance, NPNVSupportsXEmbedBool, &support);
        *((NPBool *) value) = support;
        break;
    default:
        return NPERR_GENERIC_ERROR;
    }

    return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    return NPERR_GENERIC_ERROR;
}

NPError
NPN_GetURLNotify (NPP instance, const char *url, const char *target,
                  void *notifyData)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);

    return browser_funcs->geturlnotify (instance, url, target, notifyData);
}

NPError
NPN_GetURL (NPP instance, const char *url, const char *target)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);

    return browser_funcs->geturl (instance, url, target);
}

NPError
NPN_PostURLNotify (NPP instance, const char *url, const char *window,
                   uint32_t len, const char *buf, NPBool file, void *notifyData)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);

    return browser_funcs->posturlnotify (instance, url, window, len, buf, file, notifyData);
}

NPError
NPN_PostURL(NPP instance, const char *url, const char *window,
            uint32_t len, const char *buf, NPBool file)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);

    return browser_funcs->posturl (instance, url, window, len, buf, file);
}

NPError
NPN_RequestRead(NPStream *stream, NPByteRange *rangeList)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);

    return browser_funcs->requestread (stream, rangeList);
}

NPError
NPN_NewStream(NPP instance, NPMIMEType type, const char *target,
              NPStream **stream)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);

    return browser_funcs->newstream (instance, type, target, stream);
}

int32_t NPN_Write(NPP instance, NPStream *stream, int32_t len, void *buffer)
{
    g_return_val_if_fail (browser_funcs != NULL, 0);

    return browser_funcs->write (instance, stream, len, buffer);
}

NPError
NPN_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);

    return browser_funcs->destroystream (instance, stream, reason);
}

void
NPN_Status(NPP instance, const char *message)
{
    g_return_if_fail (browser_funcs != NULL);

    browser_funcs->status (instance, message);
}

const char *NPN_UserAgent(NPP instance)
{
    g_return_val_if_fail (browser_funcs != NULL, NULL);

    return browser_funcs->uagent(instance);
}

void *NPN_MemAlloc(uint32_t size)
{
    g_return_val_if_fail (browser_funcs != NULL, NULL);

    return browser_funcs->memalloc(size);
}

void
NPN_MemFree(void *ptr)
{
    g_return_if_fail (browser_funcs != NULL);

    browser_funcs->memfree(ptr);
}

uint32_t NPN_MemFlush (uint32_t size)
{
    g_return_val_if_fail (browser_funcs != NULL, 0);
    return browser_funcs->memflush(size);
}

void
NPN_ReloadPlugins (NPBool reloadPages)
{
    g_return_if_fail (browser_funcs != NULL);

    browser_funcs->reloadplugins(reloadPages);
}

NPError
NPN_GetValue(NPP instance, NPNVariable variable, void *value)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);
    return browser_funcs->getvalue(instance, variable, value);
}

NPError
NPN_SetValue(NPP instance, NPPVariable variable, void *value)
{
    g_return_val_if_fail (browser_funcs != NULL, NPERR_INVALID_PLUGIN_ERROR);
    return browser_funcs->setvalue(instance, variable, value);
}

void
NPN_InvalidateRect(NPP instance, NPRect *invalidRect)
{
    g_return_if_fail (browser_funcs != NULL);
    browser_funcs->invalidaterect(instance, invalidRect);
}

void
NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
    g_return_if_fail (browser_funcs != NULL);
    browser_funcs->invalidateregion(instance, invalidRegion);
}

void
NPN_ForceRedraw(NPP instance)
{
    g_return_if_fail (browser_funcs != NULL);
    browser_funcs->forceredraw(instance);
}

NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name)
{
    g_return_val_if_fail (browser_funcs != NULL, 0);
    return browser_funcs->getstringidentifier(name);
}

void
NPN_GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount,
                              NPIdentifier *identifiers)
{
    g_return_if_fail (browser_funcs != NULL);
    return browser_funcs->getstringidentifiers(names, nameCount, identifiers);
}


NPIdentifier NPN_GetIntIdentifier(int32_t id)
{
    g_return_val_if_fail (browser_funcs != NULL, 0);
    return browser_funcs->getintidentifier(id);
}

bool
NPN_IdentifierIsString(NPIdentifier identifier)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->identifierisstring(identifier);
}

NPUTF8 *NPN_UTF8FromIdentifier(NPIdentifier identifier)
{
    g_return_val_if_fail (browser_funcs != NULL, NULL);
    return browser_funcs->utf8fromidentifier(identifier);
}

int32_t NPN_IntFromIdentifier(NPIdentifier identifier)
{
    g_return_val_if_fail (browser_funcs != NULL, 0);
    return browser_funcs->intfromidentifier(identifier);
}

NPObject *
NPN_CreateObject(NPP instance, NPClass *klass)
{
    g_return_val_if_fail (browser_funcs != NULL, NULL);
    return browser_funcs->createobject (instance, klass);
}

NPObject *NPN_RetainObject(NPObject *obj)
{
    g_return_val_if_fail (browser_funcs != NULL, NULL);
    return browser_funcs->retainobject(obj);
}

void
NPN_ReleaseObject(NPObject *obj)
{
    g_return_if_fail (browser_funcs != NULL);
    return browser_funcs->releaseobject(obj);
}

bool
NPN_Invoke (NPP npp, NPObject *obj, NPIdentifier methodName,
                 const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->invoke(npp, obj, methodName, args, argCount, result);
}

bool
NPN_InvokeDefault (NPP npp, NPObject *obj, const NPVariant *args,
                        uint32_t argCount, NPVariant *result)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->invokeDefault(npp, obj, args, argCount, result);
}

bool
NPN_Evaluate(NPP npp, NPObject *obj, NPString *script,
                  NPVariant *result)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->evaluate(npp, obj, script, result);
}

bool
NPN_GetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName,
                     NPVariant *result)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->getproperty(npp, obj, propertyName, result);
}

bool
NPN_SetProperty (NPP npp, NPObject *obj, NPIdentifier propertyName,
                      const NPVariant *value)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->setproperty(npp, obj, propertyName, value);
}

bool
NPN_RemoveProperty (NPP npp, NPObject *obj, NPIdentifier propertyName)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->removeproperty(npp, obj, propertyName);
}

bool
NPN_Enumerate (NPP npp, NPObject *obj, NPIdentifier **identifier,
                    uint32_t *count)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->enumerate(npp, obj, identifier, count);
}

bool
NPN_Construct (NPP npp, NPObject *obj, const NPVariant *args,
                    uint32_t argCount, NPVariant *result)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->construct(npp, obj, args, argCount, result);
}

bool
NPN_HasProperty (NPP npp, NPObject *obj, NPIdentifier propertyName)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->hasproperty(npp, obj, propertyName);
}

bool
NPN_HasMethod (NPP npp, NPObject *obj, NPIdentifier methodName)
{
    g_return_val_if_fail (browser_funcs != NULL, false);
    return browser_funcs->hasmethod(npp, obj, methodName);
}

void
NPN_ReleaseVariantValue (NPVariant *variant)
{
    g_return_if_fail (browser_funcs != NULL);
    browser_funcs->releasevariantvalue(variant);
}

void
NPN_SetException (NPObject *obj, const NPUTF8 *message)
{
    g_return_if_fail (browser_funcs != NULL);
    browser_funcs->setexception(obj, message);
}

