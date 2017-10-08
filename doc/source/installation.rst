Installation
============

Snuffleupagus is tested against various PHP 7+ versions: XXX

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
    ./configure --with-snuffleupagus
    make
    make install

This should install the ``snuffleupagus.so`` file in your extension directory.
The final step is adding a load directive to ``php.ini``
and to specify the location of the :doc:`configuration file <config>`:

::

    extension=snuffleupagus.so
    sp.configuration_file=/etc/php/conf.d/snuffleupagus.ini

Upgrading
---------

Upgrading the Snuffleupagus is as simple as recompiling it (or using a binary), replacing the file and restarting your webserver.
