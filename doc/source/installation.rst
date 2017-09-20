Installation
============

Snuffleupagus is tested against various PHP 7+ versions: XXX

Manual installation
-------------------

Depending on the system, we might already offer binary packages.
You can check our :doc:`download`. In that case you only need to activate
the extension inside your ``php.ini`` and to configure it.


Quickstart
^^^^^^^^^^

::

    git clone https://github.com/nbs-system/snuffleupagus
    cd snuffleupagus
    phpize
    ./configure
    make
    make install

This should install ``snuffleupagus.so`` file in your extension directory. The final step is adding a load directive to ``php.ini``::

    extension=snuffleupagus.so

Upgrading
---------

Upgrading the Snuffleupagus is as simple as recompiling it (or using a binary), replacing the file and restarting your webserver.
