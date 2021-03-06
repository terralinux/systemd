#  This file is part of systemd.
#
#  Copyright 2010 Lennart Poettering
#
#  systemd is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  systemd is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with systemd; If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ(2.63)

AC_INIT([systemd],[27],[systemd-devel@lists.freedesktop.org])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE

AM_INIT_AUTOMAKE([foreign 1.11 -Wall -Wno-portability silent-rules tar-pax subdir-objects dist-bzip2])

AC_SUBST(PACKAGE_URL, [http://www.freedesktop.org/wiki/Software/systemd])

AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED([CANONICAL_HOST], "$host", [Canonical host string.])
AS_IF([test "x$host_cpu" = "xmips" || test "x$host_cpu" = "xmipsel" ||
       test "x$host_cpu" = "xmips64" || test "x$host_cpu" = "xmips64el"],
      [AC_DEFINE(ARCH_MIPS, [], [Whether on mips arch])])

AM_SILENT_RULES([yes])

AC_CHECK_PROG([STOW], [stow], [yes], [no])

AS_IF([test "x$STOW" = "xyes" && test -d /usr/local/stow], [
        AC_MSG_NOTICE([*** Found /usr/local/stow: default install prefix set to /usr/local/stow/${PACKAGE_NAME}-${PACKAGE_VERSION} ***])
        ac_default_prefix="/usr/local/stow/${PACKAGE_NAME}-${PACKAGE_VERSION}"
])

AC_PROG_MKDIR_P
AC_PROG_LN_S
AC_PROG_SED

AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_PROG_GCC_TRADITIONAL

AC_CHECK_TOOL(OBJCOPY, objcopy)
AC_CHECK_TOOL(STRINGS, strings)

CC_CHECK_CFLAGS_APPEND([ \
        -pipe \
        -Wall \
        -W \
        -Wextra \
        -Wno-inline \
        -Wvla \
        -Wundef \
        -Wformat=2 \
        -Wlogical-op \
        -Wsign-compare \
        -Wformat-security \
        -Wmissing-include-dirs \
        -Wformat-nonliteral \
        -Wold-style-definition \
        -Wpointer-arith \
        -Winit-self \
        -Wdeclaration-after-statement \
        -Wfloat-equal \
        -Wmissing-prototypes \
        -Wstrict-prototypes \
        -Wredundant-decls \
        -Wmissing-declarations \
        -Wmissing-noreturn \
        -Wshadow \
        -Wendif-labels \
        -Wcast-align \
        -Wstrict-aliasing=2 \
        -Wwrite-strings \
        -Wno-long-long \
        -Wno-overlength-strings \
        -Wno-unused-parameter \
        -Wno-missing-field-initializers \
        -Wno-unused-result \
        -Wp,-D_FORTIFY_SOURCE=2 \
        -ffast-math \
        -fno-common \
        -fdiagnostics-show-option \
        -fno-strict-aliasing \
        -fvisibility=hidden \
        -ffunction-sections \
        -fdata-sections \
        -Wl,--as-needed \
        -Wl,--gc-sections])

LT_PREREQ(2.2)
LT_INIT

AC_SEARCH_LIBS([clock_gettime], [rt], [], [AC_MSG_ERROR([*** POSIX RT library not found])])
AC_SEARCH_LIBS([dlsym], [dl], [], [AC_MSG_ERROR([*** Dynamic linking loader library not found])])
AC_SEARCH_LIBS([cap_init], [cap], [], [AC_MSG_ERROR([*** POSIX caps library not found])])
AC_CHECK_HEADERS([sys/capability.h], [], [AC_MSG_ERROR([*** POSIX caps headers not found])])

# This makes sure pkg.m4 is available.
m4_pattern_forbid([^_?PKG_[A-Z_]+$],[*** pkg.m4 missing, please install pkg-config])

PKG_CHECK_MODULES(UDEV, [ libudev >= 160 ])
AC_SUBST(UDEV_CFLAGS)
AC_SUBST(UDEV_LIBS)

PKG_CHECK_MODULES(DBUS, [ dbus-1 >= 1.3.2 ])
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)

have_selinux=no
AC_ARG_ENABLE(selinux, AS_HELP_STRING([--disable-selinux], [Disable optional SELINUX support]))
if test "x$enable_selinux" != "xno"; then
        PKG_CHECK_MODULES(SELINUX, [ libselinux ],
                [AC_DEFINE(HAVE_SELINUX, 1, [Define if SELinux is available]) have_selinux=yes], have_selinux=no)
        AC_SUBST(SELINUX_CFLAGS)
        AC_SUBST(SELINUX_LIBS)
        if test "x$have_selinux" = xno -a "x$enable_selinux" = xyes; then
                AC_MSG_ERROR([*** SELinux support requested but libraries not found])
        fi
fi
AM_CONDITIONAL(HAVE_SELINUX, [test "$have_selinux" = "yes"])

AC_ARG_ENABLE([tcpwrap],
        AS_HELP_STRING([--disable-tcpwrap],[Disable optional TCP wrappers support]),
                [case "${enableval}" in
                        yes) have_tcpwrap=yes ;;
                        no) have_tcpwrap=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-tcpwrap) ;;
                esac],
                [have_tcpwrap=auto])

if test "x${have_tcpwrap}" != xno ; then
        ACX_LIBWRAP
        if test "x${LIBWRAP_LIBS}" = x ; then
                if test "x$have_tcpwrap" = xyes ; then
                        AC_MSG_ERROR([*** TCP wrappers support not found.])
                fi
                have_tcpwrap=no
        else
                have_tcpwrap=yes
        fi
else
        LIBWRAP_LIBS=
fi
AC_SUBST(LIBWRAP_LIBS)

AC_ARG_ENABLE([pam],
        AS_HELP_STRING([--disable-pam],[Disable optional PAM support]),
                [case "${enableval}" in
                        yes) have_pam=yes ;;
                        no) have_pam=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-pam) ;;
                esac],
                [have_pam=auto])

if test "x${have_pam}" != xno ; then
        AC_CHECK_HEADERS(
                [security/pam_modules.h security/pam_modutil.h security/pam_ext.h],
                [have_pam=yes],
                [if test "x$have_pam" = xyes ; then
                        AC_MSG_ERROR([*** PAM headers not found.])
                fi])

        AC_CHECK_LIB(
                [pam],
                [pam_syslog],
                [have_pam=yes],
                [if test "x$have_pam" = xyes ; then
                        AC_MSG_ERROR([*** libpam not found.])
                fi])

        if test "x$have_pam" = xyes ; then
                PAM_LIBS="-lpam -lpam_misc"
                AC_DEFINE(HAVE_PAM, 1, [PAM available])
        else
                have_pam=no
        fi
else
        PAM_LIBS=
fi
AC_SUBST(PAM_LIBS)
AM_CONDITIONAL([HAVE_PAM], [test "x$have_pam" != xno])

AC_ARG_ENABLE([audit],
        AS_HELP_STRING([--disable-audit],[Disable optional AUDIT support]),
                [case "${enableval}" in
                        yes) have_audit=yes ;;
                        no) have_audit=no ;;
                        *) AC_MSG_ERROR(bad value ${enableval} for --disable-audit) ;;
                esac],
                [have_audit=auto])

if test "x${have_audit}" != xno ; then
        AC_CHECK_HEADERS(
                [libaudit.h],
                [have_audit=yes],
                [if test "x$have_audit" = xyes ; then
                        AC_MSG_ERROR([*** AUDIT headers not found.])
                fi])

        AC_CHECK_LIB(
                [audit],
                [audit_open],
                [have_audit=yes],
                [if test "x$have_audit" = xyes ; then
                        AC_MSG_ERROR([*** libaudit not found.])
                fi])

        if test "x$have_audit" = xyes ; then
                AUDIT_LIBS="-laudit"
                AC_DEFINE(HAVE_AUDIT, 1, [AUDIT available])
        else
                have_audit=no
        fi
else
        AUDIT_LIBS=
fi
AC_SUBST(AUDIT_LIBS)

have_libcryptsetup=no
AC_ARG_ENABLE(libcryptsetup, AS_HELP_STRING([--disable-libcryptsetup], [disable libcryptsetup tools]))
if test "x$enable_libcryptsetup" != "xno"; then
        PKG_CHECK_MODULES(LIBCRYPTSETUP, [ libcryptsetup ],
                [AC_DEFINE(HAVE_LIBCRYPTSETUP, 1, [Define if libcryptsetup is available]) have_libcryptsetup=yes], have_libcryptsetup=no)
        AC_SUBST(LIBCRYPTSETUP_CFLAGS)
        AC_SUBST(LIBCRYPTSETUP_LIBS)
        if test "x$have_libcryptsetup" = xno -a "x$enable_libcryptsetup" = xyes; then
                AC_MSG_ERROR([*** libcryptsetup support requested but libraries not found])
        fi
fi
AM_CONDITIONAL(HAVE_LIBCRYPTSETUP, [test "$have_libcryptsetup" = "yes"])

have_binfmt=no
AC_ARG_ENABLE(binfmt, AS_HELP_STRING([--disable-binfmt], [disable binfmt tool]))
if test "x$enable_binfmt" != "xno"; then
	have_binfmt=yes
fi
AM_CONDITIONAL(ENABLE_BINFMT, [test "$have_binfmt" = "yes"])

have_gtk=no
AC_ARG_ENABLE(gtk, AS_HELP_STRING([--disable-gtk], [disable GTK tools]))
if test "x$enable_gtk" != "xno"; then
        PKG_CHECK_MODULES(GTK, [ gtk+-2.0 glib-2.0 > 2.26 gio-unix-2.0 ],
                [AC_DEFINE(HAVE_GTK, 1, [Define if GTK is available]) have_gtk=yes], have_gtk=no)
        AC_SUBST(GTK_CFLAGS)
        AC_SUBST(GTK_LIBS)
        if test "x$have_gtk" = xno -a "x$enable_gtk" = xyes; then
                AC_MSG_ERROR([*** gtk support requested but libraries not found])
        fi
fi
AM_CONDITIONAL(HAVE_GTK, [test "$have_gtk" = "yes"])

if test "$have_gtk" = "yes"; then
        PKG_CHECK_MODULES(LIBNOTIFY, [ libnotify ])
        PKG_CHECK_EXISTS([ libnotify >= 0.7.0 ], [ libnotify07=yes ])

        AC_SUBST(LIBNOTIFY_CFLAGS)
        AC_SUBST(LIBNOTIFY_LIBS)
fi
AM_CONDITIONAL(LIBNOTIFY07, [ test "$libnotify07" = "yes" ])

AM_PROG_VALAC([0.10])
AC_SUBST(VAPIDIR)
AM_CONDITIONAL(HAVE_VALAC, test x"$VALAC" != x)

AC_PATH_PROG([XSLTPROC], [xsltproc])
AM_CONDITIONAL(HAVE_XSLTPROC, test x"$XSLTPROC" != x)

AC_PATH_PROG([M4], [m4])

AC_ARG_WITH(distro, AS_HELP_STRING([--with-distro=DISTRO],[Specify the distribution to target: One of fedora, suse, debian, ubuntu, arch, gentoo, slackware, altlinux or other]))
if test "z$with_distro" = "z"; then
        if test "$cross_compiling" = yes; then
                AC_MSG_WARN([Target distribution cannot be reliably detected when cross-compiling. You should specify it with --with-distro (see $0 --help for recognized distros)])
        else
                test -f "/etc/redhat-release" && with_distro="fedora"
                test -f "/etc/SuSE-release" && with_distro="suse"
                test -f "/etc/debian_version" &&  with_distro="debian"
                test -f "/etc/arch-release" && with_distro="arch"
                test -f "/etc/gentoo-release" && with_distro="gentoo"
                test -f "/etc/slackware-version" && with_distro="slackware"
                test -f "/etc/frugalware-release" && with_distro="frugalware"
                test -f "/etc/altlinux-release" && with_distro="altlinux"
                test -f "/etc/mandriva-release" && with_distro="mandriva"
                test -f "/etc/meego-release" && with_distro="meego"
                test -f "/etc/angstrom-version" && with_distro="angstrom"
                test -f "/etc/terra-release" && with_distro="terra"
                if test "x`lsb_release -is 2>/dev/null`" = "xUbuntu"; then
                        with_distro="ubuntu"
                fi
        fi
        if test "z$with_distro" = "z"; then
                with_distro=`uname -s`
        fi
fi
with_distro=`echo ${with_distro} | tr '[[:upper:]]' '[[:lower:]]' `
AC_DEFINE_UNQUOTED(DISTRIBUTION, ["${with_distro}"], [Target Distribution])

# Location of the init scripts as mandated by LSB
SYSTEM_SYSVINIT_PATH=/etc/init.d
SYSTEM_SYSVRCND_PATH=/etc/rc.d

M4_DISTRO_FLAG=

case $with_distro in
        fedora)
                SYSTEM_SYSVINIT_PATH=/etc/rc.d/init.d
                AC_DEFINE(TARGET_FEDORA, [], [Target is Fedora/RHEL])
                M4_DISTRO_FLAG=-DTARGET_FEDORA=1
		have_plymouth=true
                ;;
        suse)
                SYSTEM_SYSVRCND_PATH=/etc/init.d
                AC_DEFINE(TARGET_SUSE, [], [Target is openSUSE/SLE])
                M4_DISTRO_FLAG=-DTARGET_SUSE=1
		have_plymouth=true
                ;;
        debian)
                SYSTEM_SYSVRCND_PATH=/etc
                AC_DEFINE(TARGET_DEBIAN, [], [Target is Debian])
                M4_DISTRO_FLAG=-DTARGET_DEBIAN=1
                ;;
        ubuntu)
                SYSTEM_SYSVRCND_PATH=/etc
                AC_DEFINE(TARGET_UBUNTU, [], [Target is Ubuntu])
                M4_DISTRO_FLAG=-DTARGET_UBUNTU=1
                ;;
        arch)
                SYSTEM_SYSVINIT_PATH=/etc/rc.d
                SYSTEM_SYSVRCND_PATH=/etc
                AC_DEFINE(TARGET_ARCH, [], [Target is ArchLinux])
                M4_DISTRO_FLAG=-DTARGET_ARCH=1
                ;;
        gentoo)
                SYSTEM_SYSVINIT_PATH=
                SYSTEM_SYSVRCND_PATH=
                AC_DEFINE(TARGET_GENTOO, [], [Target is Gentoo])
                M4_DISTRO_FLAG=-DTARGET_GENTOO=1
                ;;
        slackware)
                SYSTEM_SYSVINIT_PATH=/etc/rc.d/init.d
                AC_DEFINE(TARGET_SLACKWARE, [], [Target is Slackware])
                M4_DISTRO_FLAG=-DTARGET_SLACKWARE=1
                ;;
        frugalware)
                SYSTEM_SYSVINIT_PATH=/etc/rc.d
                AC_DEFINE(TARGET_FRUGALWARE, [], [Target is Frugalware])
                M4_DISTRO_FLAG=-DTARGET_FRUGALWARE=1
                have_plymouth=true
                ;;
        altlinux)
                SYSTEM_SYSVINIT_PATH=/etc/rc.d/init.d
                AC_DEFINE(TARGET_ALTLINUX, [], [Target is ALTLinux])
                M4_DISTRO_FLAG=-DTARGET_ALTLINUX=1
		have_plymouth=true
                ;;
        mandriva)
                SYSTEM_SYSVINIT_PATH=/etc/rc.d/init.d
                AC_DEFINE(TARGET_MANDRIVA, [], [Target is Mandriva])
                M4_DISTRO_FLAG=-DTARGET_MANDRIVA=1
		have_plymouth=true
                ;;
        meego)
                SYSTEM_SYSVINIT_PATH=
                SYSTEM_SYSVRCND_PATH=
                AC_DEFINE(TARGET_MEEGO, [], [Target is MeeGo])
                M4_DISTRO_FLAG=-DTARGET_MEEGO=1
		;;
        angstrom)
                SYSTEM_SYSVRCND_PATH=/etc
                AC_DEFINE(TARGET_ANGSTROM, [], [Target is Ångström])
                M4_DISTRO_FLAG=-DTARGET_ANGSTROM=1
                ;;
        terra)
                SYSTEM_SYSVINIT_PATH=/etc/rc.d
                SYSTEM_SYSVRCND_PATH=/etc
                AC_DEFINE(TARGET_TERRA, [], [Target is Terralinux])
                M4_DISTRO_FLAG=-DTARGET_TERRA=1
                ;;
        other)
                ;;
        *)
                AC_MSG_ERROR([Your distribution (${with_distro}) is not yet supported, SysV init scripts could not be found! (patches welcome); you can specify --with-distro=other to skip this check])
                ;;
esac

AC_ARG_WITH([sysvinit-path],
	[AS_HELP_STRING([--with-sysvinit-path=PATH],
		[Specify the path to where the SysV init scripts are located @<:@default=based on distro@:>@])],
	[SYSTEM_SYSVINIT_PATH="$withval"],
	[])

AC_ARG_WITH([sysvrcd-path],
        [AS_HELP_STRING([--with-sysvrcd-path=PATH],
                [Specify the path to the base directory for the SysV rcN.d directories @<:@default=based on distro@:>@])],
        [SYSTEM_SYSVRCND_PATH="$withval"],
        [])

AC_SUBST(SYSTEM_SYSVINIT_PATH)
AC_SUBST(SYSTEM_SYSVRCND_PATH)
AC_SUBST(M4_DISTRO_FLAG)

if test "x${SYSTEM_SYSVINIT_PATH}" != "x" -a "x${SYSTEM_SYSVRCND_PATH}" != "x"; then
        AC_DEFINE(HAVE_SYSV_COMPAT, [], [SysV init scripts and rcN.d links are supported.])
        SYSTEM_SYSV_COMPAT="yes"
elif test "x${SYSTEM_SYSVINIT_PATH}" != "x" -o "x${SYSTEM_SYSVRCND_PATH}" != "x"; then
        AC_MSG_ERROR([*** You need both --with-sysvinit-path=PATH and --with-sysvrcd-path=PATH to enable SysV compatibility support, or both empty to disable it.])
else
        SYSTEM_SYSV_COMPAT="no"
fi

AC_ARG_WITH([tty-gid],
        [AS_HELP_STRING([--with-tty-gid=GID],
                [Specify the numeric GID of the 'tty' group])],
        [AC_DEFINE_UNQUOTED(TTY_GID, [$withval], [GID of the 'tty' group])],
        [])

AM_CONDITIONAL(TARGET_FEDORA, test x"$with_distro" = xfedora)
AM_CONDITIONAL(TARGET_SUSE, test x"$with_distro" = xsuse)
AM_CONDITIONAL(TARGET_DEBIAN, test x"$with_distro" = xdebian)
AM_CONDITIONAL(TARGET_UBUNTU, test x"$with_distro" = xubuntu)
AM_CONDITIONAL(TARGET_DEBIAN_OR_UBUNTU, test x"$with_distro" = xdebian -o x"$with_distro" = xubuntu)
AM_CONDITIONAL(TARGET_ARCH, test x"$with_distro" = xarch)
AM_CONDITIONAL(TARGET_GENTOO, test x"$with_distro" = xgentoo)
AM_CONDITIONAL(TARGET_SLACKWARE, test x"$with_distro" = xslackware)
AM_CONDITIONAL(TARGET_FRUGALWARE, test x"$with_distro" = xfrugalware)
AM_CONDITIONAL(TARGET_ALTLINUX, test x"$with_distro" = xaltlinux)
AM_CONDITIONAL(TARGET_MANDRIVA, test x"$with_distro" = xmandriva)
AM_CONDITIONAL(TARGET_MEEGO, test x"$with_distro" = xmeego)
AM_CONDITIONAL(TARGET_ANGSTROM, test x"$with_distro" = xangstrom)
AM_CONDITIONAL(TARGET_TERRA, test x"$with_distro" = xterra)

AM_CONDITIONAL(HAVE_PLYMOUTH, test -n "$have_plymouth")
AM_CONDITIONAL(HAVE_SYSV_COMPAT, test "$SYSTEM_SYSV_COMPAT" = "yes")

AC_ARG_WITH([dbuspolicydir],
        AS_HELP_STRING([--with-dbuspolicydir=DIR], [D-Bus policy directory]),
        [],
        [with_dbuspolicydir=`pkg-config --variable=sysconfdir dbus-1`/dbus-1/system.d])

AC_ARG_WITH([dbussessionservicedir],
        AS_HELP_STRING([--with-dbussessionservicedir=DIR], [D-Bus session service directory]),
        [],
        [with_dbussessionservicedir=`pkg-config --variable=session_bus_services_dir dbus-1`])

AC_ARG_WITH([dbussystemservicedir],
        AS_HELP_STRING([--with-dbussystemservicedir=DIR], [D-Bus system service directory]),
        [],
        [with_dbussystemservicedir=`pkg-config --variable=session_bus_services_dir dbus-1`/../system-services])

AC_ARG_WITH([dbusinterfacedir],
        AS_HELP_STRING([--with-dbusinterfacedir=DIR], [D-Bus interface directory]),
        [],
        [with_dbusinterfacedir=`pkg-config --variable=session_bus_services_dir dbus-1`/../interfaces])

AC_ARG_WITH([udevrulesdir],
        AS_HELP_STRING([--with-udevrulesdir=DIR], [Diectory for udev rules]),
        [],
        [with_udevrulesdir=`pkg-config --variable=udevdir udev`/rules.d])

AC_ARG_WITH([pamlibdir],
        AS_HELP_STRING([--with-pamlibdir=DIR], [Diectory for PAM modules]),
        [],
        [with_pamlibdir=/lib/`$CC -print-multi-os-directory`/security])

AC_ARG_WITH([rootdir],
        AS_HELP_STRING([--with-rootdir=DIR], [Root directory for files necessary for boot]),
        [],
        [with_rootdir=${ac_default_prefix}])

AC_SUBST([dbuspolicydir], [$with_dbuspolicydir])
AC_SUBST([dbussessionservicedir], [$with_dbussessionservicedir])
AC_SUBST([dbussystemservicedir], [$with_dbussystemservicedir])
AC_SUBST([dbusinterfacedir], [$with_dbusinterfacedir])
AC_SUBST([udevrulesdir], [$with_udevrulesdir])
AC_SUBST([pamlibdir], [$with_pamlibdir])
AC_SUBST([rootdir], [$with_rootdir])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "
        $PACKAGE_NAME $VERSION

        Distribution:            ${with_distro}
        SysV compatibility:      ${SYSTEM_SYSV_COMPAT}
        SysV init scripts:       ${SYSTEM_SYSVINIT_PATH}
        SysV rc?.d directories:  ${SYSTEM_SYSVRCND_PATH}
        Gtk:                     ${have_gtk}
        libcryptsetup:           ${have_libcryptsetup}
        tcpwrap:                 ${have_tcpwrap}
        PAM:                     ${have_pam}
        AUDIT:                   ${have_audit}
        SELinux:                 ${have_selinux}
        binfmt:                  ${have_binfmt}
        prefix:                  ${prefix}
        root dir:                ${with_rootdir}
        udev rules dir:          ${with_udevrulesdir}
        pam modules dir:         ${with_pamlibdir}
        dbus policy dir:         ${with_dbuspolicydir}
        dbus session dir:        ${with_dbussessionservicedir}
        dbus system dir:         ${with_dbussystemservicedir}
        dbus interfaces dir:     ${with_dbusinterfacedir}
"
