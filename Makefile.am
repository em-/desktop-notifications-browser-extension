SUBDIRS = npapi-plugin po

if WITH_CHROMIUM
SUBDIRS += chromium-extension
endif
if WITH_GOOGLE_CHROME
SUBDIRS += chromium-extension
endif

ACLOCAL_AMFLAGS = -I m4

CLEANFILES = desktop-notifications.pem

EXTRA_DIST = autogen.sh
