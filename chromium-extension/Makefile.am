CLEANFILES = res/*
EXTRA_DIST =

extensiondir = $(libdir)/desktop-notifications-chromium-common
extension_DATA=res/desktop-notifications.crx

clean-extension-package:
	rm -f $(CLEANFILES)

SCRIPT_FILES = \
	$(wildcard *.html) \
	$(wildcard *.js)

noinst_DATA = $(SCRIPT_FILES)

NPAPI_PLUGIN=$(top_builddir)/npapi-plugin/src/.libs/libdesktopnotifications_npapi_plugin.so

res/desktop-notifications.crx: $(SCRIPT_FILES) manifest.json desktop-notifications.json $(NPAPI_PLUGIN)
	cp -f $(NPAPI_PLUGIN) .
	if [ ! -f ../desktop-notifications.pem ]; \
	then \
		openssl genrsa 1024 > ../desktop-notifications.pem; \
	fi
	$(srcdir)/crxmake.sh $(abs_builddir) ../desktop-notifications.pem
	mv res/chromium-extension.crx res/desktop-notifications.crx

if WITH_CHROMIUM
chromiummanifestdir = $(datadir)/chromium/extensions/
chromiummanifest_DATA = $(wildcard $(top_builddir)/chromium-extension/res/*.json)
endif

if WITH_GOOGLE_CHROME
googlechromemanifestdir = $(datadir)/google-chrome/extensions/
googlechromemanifest_DATA = $(wildcard $(top_builddir)/chromium-extension/res/*.json)
endif

%.json: %.json.in
	sed -e "s|\@EXTDIR\@|$(extensiondir)/desktop-notifications.crx|" \
	    -e "s|\@PACKAGE_URL\@|$(PACKAGE_URL)|"                       \
	    -e "s|\@VERSION\@|$(VERSION)|" $< > $@

CLEANFILES += \
	manifest.json \
	desktop-notifications.json \
	libdesktopnotifications_npapi_plugin.so \
	chromium-extension.crx

EXTRA_DIST += \
	manifest.json.in \
	desktop-notifications.json.in \
	crxmake.sh \
	$(SCRIPT_FILES)
