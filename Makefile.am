if WITH_CHROMIUM
CHROMIUM_SUBDIR = chromium-extension
endif
if WITH_GOOGLE_CHROME
CHROMIUM_SUBDIR = chromium-extension
endif

SUBDIRS = npapi-plugin po $(CHROMIUM_SUBDIR)

ACLOCAL_AMFLAGS = -I m4

CLEANFILES = desktop-notifications.pem

EXTRA_DIST = autogen.sh
