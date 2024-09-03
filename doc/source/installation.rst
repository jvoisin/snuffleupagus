Installation
============

Snuffleupagus is tested against `various PHP 7+ versions <https://travis-ci.org/jvoisin/snuffleupagus/>`_.

Manual installation
-------------------

Depending on the system, we might already offer binary packages.
You can check our :doc:`download`. In that case you only need to activate
the extension inside your ``php.ini`` and to configure it.

Requirements
^^^^^^^^^^^^

The only dependency (at least on Debian) to compile Snuffleupagus is
`php7.0-dev <https://packages.debian.org/search?keywords=php7.0-dev>`_ or onwards.

Quickstart
^^^^^^^^^^

::

    git clone https://github.com/jvoisin/snuffleupagus
    cd snuffleupagus/src
    phpize
    ./configure --enable-snuffleupagus
    make
    make install

This should install the ``snuffleupagus.so`` file in your extension directory.
The final step is adding an extension loading directive, and to specify the
location of the :doc:`configuration file <config>`, either in a
``conf.d/20-snuffleupagus.ini`` file, or directly in you ``php.ini`` if you
prefer:

::

    extension=snuffleupagus.so

    # This is only an example,
    # you can place your rules wherever you want.
    sp.configuration_file=/etc/php/conf.d/snuffleupagus.rules


Be careful, on some distribution, there are separate configurations for
cli/fpm/cgi/… be sure to edit the right one.

If you're using `Gentoo <https://gentoo.org>`__, you might encounter the
following error:

::

  $ make                                                                                                                                         
  $ /bin/sh /root/snuffleupagus-0.5.0/src/libtool --mode=compile cc  -I. -I/root/snuffleupagus-0.5.0/src -DPHP_ATOM_INC -I/root/snuffleupagus-0.5.0/src/include -I/root/snuffleupagus-0.5.0/src/main -I/root/snuffleupagus-0.5.0/src -I/usr/lib64/php7.3/include/php -I/usr/lib64/php7.3/include/php/main -I/usr/lib64/php7.3/include/php/TSRM -I/usr/lib64/php7.3/include/php/Zend -I/usr/lib64/php7.3/include/php/ext -I/usr/lib64/php7.3/include/php/ext/date/lib  -DHAVE_CONFIG_H  -g -O2 -Wall -Wextra -Wno-unused-parameter -Wformat=2 -Wformat-security -D_FORTIFY_SOURCE=2 -fstack-protector   -c /root/snuffleupagus-0.5.0/src/snuffleupagus.c -o snuffleupagus.lo 
  libtool: Version mismatch error.  This is libtool 2.4.6, but the
  libtool: definition of this LT_INIT comes from an older release.
  libtool: You should recreate aclocal.m4 with macros from libtool 2.4.6
  libtool: and run autoconf again.
  make: *** [Makefile:193: snuffleupagus.lo] Error 63
  $

This is a `documented php bug <https://bugs.php.net/bug.php?id=58979>`__,
solvable via:


::

  rm -f aclocal.m4
  phpize
  aclocal && libtoolize --force && autoreconf
  ./configure --enable-snuffleupagus
  make

OpenBSD installation
--------------------

The required dependencies can be installed via ``doas pkg_add git php autoconf gmake``.
Note that ``gmake`` is required, as PHP's build system generates Makefiles 
with GNU idioms.


Heroku installation
-------------------

Heroku's official `buildpack <https://github.com/heroku/heroku-buildpack-php/>`_
uses ``Composer`` to install all dependencies required by your PHP application.
Careful with the `default set of rules
<https://github.com/jvoisin/snuffleupagus/blob/master/config/default.rules>`__,
since it might block the composer deployment, leading to the following errors:

::

  heroku[web.1]: Starting process with command `vendor/bin/heroku-php-apache2 -F fpm_custom.conf public/`
  heroku[web.1]: Stopping all processes with SIGTERM
  app[web.1]: Stopping httpd...
  app[web.1]: SIGTERM received, attempting graceful shutdown...
  app[web.1]: Stopping php-fpm...
  app[web.1]: Shutdown complete.
  heroku[web.1]: Process exited with status 143
  app[web.1]: [heroku-exec] Starting
  app[web.1]: Unable to determine Composer vendor-dir setting; is 'composer' executable on path or 'composer.phar' in current working directory?
  heroku[web.1]: Process exited with status 1
  heroku[web.1]: State changed from starting to crashed

Requirements
^^^^^^^^^^^^

To install snuffleupagus on heroku, simply follow the `documentation <https://devcenter.heroku.com/articles/php-support#custom-compile-step>`_,
and edit the ``composer.json`` file, as well as the ``Procfile`` to load the additional PHP-FPM configuration.

composer.json
"""""""""""""

::

    {
        "require": {
            "php": "~7.4.6"
        },
        "config": {
            "platform": {
                "php": "7.4.6"
            }
        },
        "scripts": {
            "compile": [
                "git clone https://github.com/jvoisin/snuffleupagus /tmp/snuffleupagus",
                "cd /tmp/snuffleupagus/src && phpize && ./configure --enable-snuffleupagus && make && make install",
                "echo 'extension=snuffleupagus.so\nsp.allow_broken_configuration=on\nsp.configuration_file=/dev/null' > /app/.heroku/php/etc/php/conf.d/999-ext-snuffleupagus.ini"
            ]
        }
    }

This configuration will compile Snuffleupagus to shared library, install it to the proper
location and specify an empty configuration in ``sp.configuration_file`` to ensure
that the ``composer`` deployment phase won't get killed by some rules.

PHP-FPM
"""""""

::

    ; ext-snuffleupagus
    php_admin_flag[sp.allow_broken_configuration] = off
    php_admin_value[sp.configuration_file]        = /app/default.rules

The final step is to point ``sp.configuration_file`` to a rule set by setting
the preference in an additional `PHP-FPM
configuration <https://devcenter.heroku.com/articles/custom-php-settings#php-fpm-configuration-include>`_.

You should now be running Snuffleupagus in PHP on heroku:

::

  app[web.1]: [05-Jul-2020 07:45:22 UTC] PHP Fatal error:  [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'exec', because its argument '$command' content (id;whoami) matched a rule in /app/public/test2.php on line 1
  app[web.1]: 10.9.226.141 - - [05/Jul/2020:07:45:22 +0000] "GET /test2.php?cmd=id;whoami HTTP/1.1" 500 - "-" "curl/7.68.0
  heroku[router]: at=info method=GET path="/test2.php?cmd=id;whoami" host=heroku-x-snuffleupagus.herokuapp.com request_id=012345678-9012-3456-7890-123456789012 fwd="1.2.3.4" dyno=web.1 connect=0ms service=7ms status=500 bytes=169 protocol=http


Upgrading
---------

Upgrading Snuffleupagus is as simple as recompiling it (or using a binary), replacing the file and restarting your webserver.
