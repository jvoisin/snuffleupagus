Debian
======

Here are the intructions to build the Debian package of snuffleupagus

Requirements
------------

I'm on Debian Stretch, I setup my compilation environment.

::

	# dpkg-buildpackage and mk-build-deps will be used
	user@debian:~$ sudo apt install dpkg-dev devscripts
	[...]

	# the final package files will be in the directory "build"
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

	# install snuffleupagus build dependencies
	# this will create and install the package snuffleupagus-build-deps that will pull required dependencies
	sudo DEBIAN_FRONTEND=noninteractive mk-build-deps -ri --tool "apt-get --no-install-recommends -y"
 

Compilation
-----------

Now we can compile the package

::

	# build the package
	# -b : build a binary package
	# -j$(nproc) : compile using $(nproc) CPU
	user@debian:~/build/snuffleupagus$ dpkg-buildpackage -b -j$(nproc) 
	[...]

	# the package is in ../
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

::

         # we then can remove the package snuffleupagus-build-deps
         user@debian:~/build/snuffleupagus$ sudo apt-get purge snuffleupagus-build-deps

Cleaning
-----------

We clean generated files during compilation

Because we didn't generate any original tar file debclean will ask you whether you want to continue

::

	user@debian:~/build/snuffleupagus$ debclean
	 Cleaning in directory .
	 This package has a Debian revision number but there does not seem to be
	 an appropriate original tar file or .orig directory in the parent directory;
	 (expected one of snuffleupagus_0.3.0.orig.tar.gz, snuffleupagus_0.3.0.orig.tar.bz2,
	 snuffleupagus_0.3.0.orig.tar.lzma,  snuffleupagus_0.3.0.orig.tar.xz or tst5.orig)
	 continue anyway? (y/n) y
	  dpkg-buildpackage --rules-target clean -rfakeroot -us -uc
	 dpkg-buildpackage: info: source package snuffleupagus
	 dpkg-buildpackage: info: source version 0.3.0-1
	 dpkg-buildpackage: info: source distribution UNRELEASED
	 dpkg-buildpackage: info: source changed by kkadosh <snuffleupagus@nbs-system.com>
	 dpkg-buildpackage: info: host architecture amd64
	  fakeroot debian/rules clean
	 debian/rules:63: warning: overriding recipe for target 'override_dh_php'
	 /usr/share/dh-php/pkg-pecl.mk:80: warning: ignoring old recipe for target 'override_dh_php'
	 dh clean --with php
	    debian/rules override_dh_auto_clean
	 make[1]: Entering directory '/home/pkger/build/snuffleupagus/github/tst5'
	 debian/rules:63: warning: overriding recipe for target 'override_dh_php'
	 /usr/share/dh-php/pkg-pecl.mk:80: warning: ignoring old recipe for target 'override_dh_php'
	 rm -rf build-7.0
	 touch clean-7.0-stamp
	 rm -f configure-7.0-stamp build-7.0-stamp install-7.0-stamp clean-7.0-stamp
	 make[1]: Leaving directory '/home/pkger/build/snuffleupagus/github/tst5'
	    dh_autoreconf_clean
	    dh_clean
