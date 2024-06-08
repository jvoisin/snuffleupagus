dnl $Id$
dnl config.m4 for extension snuffleupagus

sources="snuffleupagus.c sp_config.c sp_config_utils.c sp_harden_rand.c"
sources="$sources sp_unserialize.c sp_utils.c sp_disable_xxe.c sp_list.c"
sources="$sources sp_disabled_functions.c sp_execute.c sp_upload_validation.c"
sources="$sources sp_cookie_encryption.c sp_network_utils.c tweetnacl.c"
sources="$sources sp_config_keywords.c sp_var_parser.c sp_var_value.c sp_tree.c"
sources="$sources sp_pcre_compat.c sp_crypt.c sp_session.c sp_sloppy.c sp_wrapper.c"
sources="$sources sp_ini.c sp_php_compat.c sp_config_scanner.c sp_ifilter.c"

PHP_ARG_ENABLE(snuffleupagus, whether to enable snuffleupagus support,
[  --enable-snuffleupagus           Enable snuffleupagus support])

PHP_ARG_ENABLE(coverage, whether to enable coverage support,
[  --enable-coverage           Enable coverage support], no, no)

PHP_ARG_ENABLE(debug, whether to enable debug messages,
[  --enable-debug           Enable debug messages], no, no)

PHP_ARG_ENABLE(debug-stderr, whether to enable debug messages,
[  --enable-debug-stderr           Enable debug messages], no, no)

# AC_PROG_CC_STDC was merged in AC_PROG_CC in autoconf 2.70
m4_version_prereq([2.70], [AC_PROG_CC], [AC_PROG_CC_STDC])

CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall -Wextra -Wno-unused-parameter"
CFLAGS="$CFLAGS -Wformat=2 -Wformat-security -D_FORTIFY_SOURCE=2"
CFLAGS="$CFLAGS -fstack-protector-strong"
CFLAGS="$CFLAGS -Wmissing-format-attribute -Wmissing-noreturn -Wpointer-arith"
CFLAGS="$CFLAGS -Wstrict-prototypes -Wunused -Wwrite-strings"

LDFLAGS="$LDFLAGS `pcre2-config --libs8`"

AX_CHECK_COMPILE_FLAG([-Wl,-z,relro,-z,now], [LDFLAGS="$LDFLAGS -Wl,-z,relro,-z,now"], {}, [-Werror])

if test "$PHP_DEBUG" = "yes"; then
  AC_DEFINE(SP_DEBUG, 1, [Enable SP debug messages])
  CFLAGS="$CFLAGS -g -ggdb -O0"
  if test "$PHP_DEBUG_STDERR" = "yes"; then
    AC_DEFINE(SP_DEBUG_STDERR, 1, [Print SP debug messages to stderr])
  fi
fi

AC_MSG_CHECKING([for NTS])
if test "$PHP_THREAD_SAFETY" != "no"; then
  AC_MSG_ERROR([ZTS (thread safe) is not supported, please use NTS (standard) build of PHP])
else
  AC_MSG_RESULT([ok])
fi

AC_CHECK_LIB(pcre, pcre_compile, AC_DEFINE(HAVE_PCRE, 1, [have pcre]))

if test "$PHP_SNUFFLEUPAGUS" = "yes"; then
   if test "$PHP_COVERAGE" = "yes"; then
      CFLAGS="$CFLAGS --coverage -O0 -g"
   fi
   PHP_NEW_EXTENSION(snuffleupagus, $sources, $ext_shared,-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi

# Uncommend once we don't support Ubuntu Focal Fossa (20.04)
#PHP_PROG_RE2C([2.0])

PHP_ADD_MAKEFILE_FRAGMENT()
