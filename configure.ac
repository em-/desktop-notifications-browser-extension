AC_PREREQ([2.63])

AC_INIT([desktop-notifications-browser-extension],
        [0.1],
        [https://github.com/em-/desktop-notifications-browser-extension/issues],
        [desktop-notifications-browser-extension],
        [https://github.com/em-/desktop-notifications-browser-extension])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 foreign -Wno-portability no-define no-dist-gzip dist-xz tar-ustar])

AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CC_C99

AC_PROG_MKDIR_P
AC_PROG_SED

AC_DISABLE_STATIC
LT_INIT

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.50.0])

GETTEXT_PACKAGE=desktop_notifications_browser_extension
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])


PKG_CHECK_MODULES(DESKTOPNOTIFICATIONS_NPAPI_PLUGIN,
		  glib-2.0
          libnotify
		  )
GLIB_GSETTINGS


dnl ***************************************************************************
dnl Chromium extension
dnl ***************************************************************************

AC_ARG_ENABLE(chromium, 
	AC_HELP_STRING([--enable-chromium], 
		[Build chromium extension. [default=yes]]),
		[enable_chromium="no"], [enable_chromium="yes"])
if test "x$enable_chromium" = "xyes"; then
    AC_DEFINE([WITH_CHROMIUM], 1, [Build chromium extension.])
fi
AM_CONDITIONAL([WITH_CHROMIUM], [test "x$enable_chromium" = "xyes"])



AC_CONFIG_FILES([
Makefile
chromium-extension/Makefile
npapi-plugin/Makefile
npapi-plugin/src/Makefile
po/Makefile.in
])
AC_OUTPUT

AC_MSG_NOTICE([

  desktop-notifications-browser-extension $VERSION
  ----------------------

  Chromium extension  : ${enable_chromium}
])
