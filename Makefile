.DEFAULT_GOAL=help
.PHONY=help

clean:  ## clean everything
	make -C src clean
	cd src; phpize --clean

release:  ## compile with releases flags
	cd src; phpize
	cd src; ./configure --enable-snuffleupagus
	make -C src

install: release  ## compile and install snuffleupagus
	make -C install

debug: ## compile a debug build
	cd src; phpize
	export CFLAGS="-g3 -ggdb -O1 -g"; cd src; ./configure --enable-snuffleupagus --enable-debug
	make -C src
	TEST_PHP_ARGS='-q' REPORT_EXIT_STATUS=1 make -C src test

coverage:  ## compile snuffleugpaus, and run the testsuite with coverage
	cd src; phpize
	cd src; ./configure --enable-snuffleupagus --enable-coverage
	make -C src
	rm -Rf src/COV.html
	TEST_PHP_ARGS='-q' REPORT_EXIT_STATUS=1 make -C src test
	lcov --base-directory ./src --directory ./src -c -o ./src/COV.info --rc lcov_branch_coverage=1 
	lcov --remove src/COV.info '/usr/*' --remove src/COV.info '*tweetnacl.c' -o src/COV.info --rc lcov_branch_coverage=1
	genhtml -o src/COV.html ./src/COV.info  --branch-coverage

bench: joomla  ## run the benchmark

joomla:
	if [ ! -d "joomla-cms" ]; then \
		git clone --depth 1 https://github.com/joomla/joomla-cms.git >/dev/null; \
	fi
	cd joomla-cms; composer install >/dev/null 2>/dev/null
	echo "\nWith snuffleupagus:"
	cd joomla-cms; time libraries/vendor/phpunit/phpunit/phpunit -d "extension=./src/modules/snuffleupagus.so" -d "sp.configuration_file=config/default.ini" --no-coverage >/dev/null
	echo "\nWithout snuffleupagus:"
	cd joomla-cms; time libraries/vendor/phpunit/phpunit/phpunit --no-coverage >/dev/null

packages: debian  ## produce packages

debian:
	dpkg-buildpackage -i -us -uc -tc -I -rfakeroot

help:
	@awk -F ':|##' '/^[^\t].+?:.*?##/ { printf "\033[36m%-30s\033[0m %s\n", $$1, $$NF }' $(MAKEFILE_LIST) | sort
