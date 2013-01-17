noinst_LTLIBRARIES = libdesktopnotifications_npapi_plugin.la

libdesktopnotifications_npapi_plugin_la_CPPFLAGS = \
	$(DESKTOPNOTIFICATIONS_NPAPI_PLUGIN_CFLAGS) \
	$(NPAPI_DEBUG_CFLAGS) \
	-DG_LOG_DOMAIN=\"desktop-notifications-npapi\" \
	-DXP_UNIX=1

libdesktopnotifications_npapi_plugin_la_SOURCES = \
	npapi-headers/headers/npapi.h \
	npapi-headers/headers/npfunctions.h \
	npapi-headers/headers/npruntime.h \
	npapi-headers/headers/nptypes.h \
	object.c \
	object.h \
	plugin.c \
	plugin.h

libdesktopnotifications_npapi_plugin_la_LDFLAGS = \
        -avoid-version \
	-module \
	-shared \
	-export-dynamic \
	-rpath /nowhere

libdesktopnotifications_npapi_plugin_la_LIBADD = \
	$(DESKTOPNOTIFICATIONS_NPAPI_PLUGIN_LIBS)

noinst_HEADERS = \
	npapi-headers/headers/npapi.h		\
	npapi-headers/headers/npfunctions.h	\
	npapi-headers/headers/npruntime.h	\
	npapi-headers/headers/nptypes.h
