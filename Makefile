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
	export CFLAGS="--coverage"; cd src; ./configure --enable-snuffleupagus --enable-coverage
	make -C src
	rm -Rf src/COV.html
	TEST_PHP_ARGS='-q' REPORT_EXIT_STATUS=1 make -C src test
	lcov --base-directory ./src --directory ./src -c -o ./src/COV.info --rc lcov_branch_coverage=1 
	lcov --remove src/COV.info '/usr/*' --remove src/COV.info '*tweetnacl.c' -o src/COV.info --rc lcov_branch_coverage=1
	genhtml -o src/COV.html ./src/COV.info  --branch-coverage

tests: joomla

joomla:
	if [ ! -d "joomla-cms" ]; then \
		git clone --depth 1 https://github.com/joomla/joomla-cms.git >/dev/null; \
	fi
	cd joomla-cms; composer install >/dev/null 2>/dev/null
	echo "\nWith snuffleupagus:"
	cd joomla-cms; time libraries/vendor/phpunit/phpunit/phpunit -d "extension=./src/modules/snuffleupagus.so" -d "sp.configuration_file=config/default.ini" --no-coverage >/dev/null
	echo "\nWithout snuffleupagus:"
	cd joomla-cms; time libraries/vendor/phpunit/phpunit/phpunit --no-coverage >/dev/null

packages: debian

debian:
	dpkg-buildpackage -i -us -uc -tc -I -rfakeroot
