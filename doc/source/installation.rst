Installation
============

Snuffleupagus is tested against `various PHP 7+ versions <https://travis-ci.org/nbs-system/snuffleupagus/>`_.

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

    git clone https://github.com/nbs-system/snuffleupagus
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
cli/fmp/cgi/… be sure to edit the right one.

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


Upgrading
---------

Upgrading the Snuffleupagus is as simple as recompiling it (or using a binary), replacing the file and restarting your webserver.
