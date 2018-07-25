Debian
======

Here are the intructions to build the debian package of snuffleupagus

PHP dependencies
----------------

Here I'm using a Debian Stretch instance so the default PHP dev package required for compilation is php7.0-dev.

The resulting package will have proper dependencies.

If I install php7.1-dev and php7.2-dev in my instance, dependencies will be ajusted.

You'll then get a single snuffleupagus package compatible with php7.0, php7.1 and php7.2

If you want to compile with php7.3-dev you'll have to add the dependency in debian/control:

::

    Build-Depends: debhelper (>= 9),
                  php7.0-dev | php7.1-dev | php7.2-dev | php7.3-dev

To avoid requiring to add specific PHP dev package version, there is a PHP dev package named `php-all-dev <https://packages.debian.org/sid/php-all-dev>`_

You would then just need to add it to Build-Depends instead :

::

    Build-Depends: debhelper (>= 9),
                  php-all-dev

**The issue is that php-all-dev only provides php7.2-dev at the moment.**


Environment
-----------

We need some packages to compile the package

::

	# dpkg-buildpackage and mk-build-deps will be used
	user@debian:~$ sudo apt install dpkg-dev devscripts
	[...]

	# the build directory is where the final package files will be
	user@debian:~$ mkdir build
	user@debian:~$ cd build

	# clone the snuffleupagus repo
	user@debian:~/build$ git clone https://github.com/nbs-system/snuffleupagus.git
	Cloning into 'snuffleupagus'...
	remote: Counting objects: 3003, done.
	remote: Compressing objects: 100% (6/6), done.
	remote: Total 3003 (delta 1), reused 2 (delta 0), pack-reused 2997
	Receiving objects: 100% (3003/3003), 9.19 MiB | 7.41 MiB/s, done.
	Resolving deltas: 100% (2032/2032), done.
	user@debian:~/build$ cd snuffleupagus/
	user@debian:~/build/snuffleupagus$ 

	user@debian:~/build/snuffleupagus$ mk-build-deps 
	Use of uninitialized value $fullname in concatenation (.) or string at /usr/bin/equivs-build line 210.
	dh_testdir
	dh_testroot
	dh_prep
	dh_testdir
	dh_testroot
	dh_install
	dh_install: Compatibility levels before 9 are deprecated (level 7 in use)
	dh_installdocs
	dh_installdocs: Compatibility levels before 9 are deprecated (level 7 in use)
	dh_installchangelogs
	dh_installchangelogs: Compatibility levels before 9 are deprecated (level 7 in use)
	dh_compress
	dh_fixperms
	dh_installdeb
	dh_installdeb: Compatibility levels before 9 are deprecated (level 7 in use)
	dh_gencontrol
	dh_md5sums
	dh_builddeb
	dpkg-deb: building package 'snuffleupagus-build-deps' in '../snuffleupagus-build-deps_0.3.0-1_all.deb'.

	The package has been created.
	Attention, the package has been created in the current directory,
	not in ".." as indicated by the message above!

	# install the dependencies
	# you can install php7.{0,1,2}-dev packages, the resulting snuffleupagus package will be built against each of these php7 versions
	user@debian:~/build/snuffleupagus$ sudo dpkg -i snuffleupagus-build-deps_0.3.0-1_all.deb 
	Selecting previously unselected package snuffleupagus-build-deps.
	(Reading database ... 29348 files and directories currently installed.)
	Preparing to unpack snuffleupagus-build-deps_0.3.0-1_all.deb ...
	Unpacking snuffleupagus-build-deps (0.3.0-1) ...
	dpkg: dependency problems prevent configuration of snuffleupagus-build-deps:
	 snuffleupagus-build-deps depends on php7.0-dev | php7.1-dev | php7.2-dev; however:
	  Package php7.0-dev is not installed.
	  Package php7.1-dev is not installed.
	  Package php7.2-dev is not installed.

	dpkg: error processing package snuffleupagus-build-deps (--install):
	 dependency problems - leaving unconfigured
	Errors were encountered while processing:
	 snuffleupagus-build-deps

	user@debian:~/build/snuffleupagus$ sudo apt --fix-broken install
	[...]

Compilation
-----------

Now we can compile the package

::

	# build the package
	# DEB_BUILD_OPTIONS="noddebs nocheck"
	# nocheck : skip PHP tests after compilation
	# noddebs : do not build a dbgsym package
	# -j$(nproc) : compile using $(nproc) CPU
	# -b : build a binary package, remove this option to build a source package
	user@debian:~/build/snuffleupagus$ DEB_BUILD_OPTIONS="noddebs" dpkg-buildpackage --no-sign -b -j$(nproc) 
	[...]

	# the package is ../
	user@debian:~/build/snuffleupagus$ ls ../snuffleupagus_0.3.0-1_amd64.deb 
	../snuffleupagus_0.3.0-1_amd64.deb

	# check the result with debc
	user@debian:~/build/snuffleupagus$ debc ../snuffleupagus_0.3.0-1_amd64.changes 
	snuffleupagus_0.3.0-1_amd64.deb
	-------------------------------
	new debian package, version 2.0.
	size 43996 bytes: control archive=1476 bytes.
	     79 bytes,     2 lines      conffiles            
	    347 bytes,    11 lines      control              
	    557 bytes,     7 lines      md5sums              
	    774 bytes,    36 lines   *  postinst             #!/bin/sh
	    852 bytes,    35 lines   *  postrm               #!/bin/sh
	    560 bytes,    21 lines   *  preinst              #!/bin/sh
	    202 bytes,     7 lines   *  prerm                #!/bin/sh
	Package: snuffleupagus
	Version: 0.3.0-1
	Architecture: amd64
	Maintainer: NBS System <snuffleupagus@nbs-system.com>
	Installed-Size: 134
	Depends: php-common (>= 1:7.0+33~), phpapi-20151012, libc6 (>= 2.14), libpcre3
	Section: php
	Priority: optional
	Homepage: https://snuffleupagus.fr
	Description: Hardening for your php7 stack
	 Snuffleupagus is cool.
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./etc/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./etc/php/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./etc/php/7.0/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./etc/php/7.0/mods-available/
	-rw-r--r-- root/root       146 2018-07-17 13:00 ./etc/php/7.0/mods-available/snuffleupagus.ini
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./etc/snuffleupagus/
	-rw-r--r-- root/root      5833 2018-07-17 13:00 ./etc/snuffleupagus/default.rules
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/lib/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/lib/php/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/lib/php/20151012/
	-rw-r--r-- root/root     96608 2018-07-17 13:00 ./usr/lib/php/20151012/snuffleupagus.so
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/lib/snuffleupagus/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/lib/snuffleupagus/scripts/
	-rw-r--r-- root/root      1370 2018-07-17 13:00 ./usr/lib/snuffleupagus/scripts/generate_rules.php
	-rwxr-xr-x root/root      1070 2018-07-17 13:00 ./usr/lib/snuffleupagus/scripts/upload_validation.py
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/share/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/share/doc/
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/share/doc/snuffleupagus/
	-rw-r--r-- root/root      1921 2018-07-17 13:00 ./usr/share/doc/snuffleupagus/README.md.gz
	-rw-r--r-- root/root      1147 2018-07-17 13:00 ./usr/share/doc/snuffleupagus/changelog.Debian.gz
	-rw-r--r-- root/root       267 2018-07-17 13:00 ./usr/share/doc/snuffleupagus/copyright
	drwxr-xr-x root/root         0 2018-07-17 13:00 ./usr/share/doc/snuffleupagus/examples/
	-rw-r--r-- root/root      1012 2018-07-17 13:00 ./usr/share/doc/snuffleupagus/examples/default.rules.gz
