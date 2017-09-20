clean:
	make -C src clean
	cd src; phpize --clean

debug:
	cd src; phpize
	export CFLAGS="-Wall -Wextra -g3 -ggdb -O1 -g -Wno-unused-function"; cd src; ./configure --enable-snuffleupagus --enable-debug
	make -C src
	TEST_PHP_ARGS='-q' REPORT_EXIT_STATUS=1 make -C src test

coverage:
	cd src; phpize
	export CFLAGS="--coverage -fprofile-arcs -ftest-coverage -O0"; export LDFLAGS="--coverage"; cd src; ./configure --enable-snuffleupagus --enable-coverage
	make -C src
	lcov --base-directory src --directory ./src --zerocounters -q   --rc lcov_branch_coverage=1
	rm -Rf src/COV.html
	TEST_PHP_ARGS='-q' REPORT_EXIT_STATUS=1 make -C src test
	lcov --base-directory ./src --directory src -c -o ./src/COV.info --rc lcov_branch_coverage=1 2>/dev/null 1>/dev/null
	lcov --remove src/COV.info '/usr/*' --remove src/COV.info '*tweetnacl.c' -o src/COV.info --rc lcov_branch_coverage=1 2>/dev/null 1>/dev/null
	genhtml -o src/COV.html ./src/COV.info  --branch-coverage

tests: joomla

joomla:
	if [ -nd "joomla-cms" ]; then git clone --depth 1 git@github.com:joomla/joomla-cms.git; fi
	cd joomla-cms; composer install
	cd joomla-cms; libraries/vendor/phpunit/phpunit/phpunit -d extension=./src/modules/snuffleupagus.so

packages: debian

debian:
	dpkg-buildpackage -i -us -uc -tc -I -rfakeroot
