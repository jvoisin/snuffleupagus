.DEFAULT_GOAL=help
.PHONY=help
SHELL = /bin/bash

SRC := src

clean:  ## clean everything
	make -C $(SRC) clean
	cd $(SRC); phpize --clean

release:  ## compile with releases flags
	cd $(SRC); phpize
	cd $(SRC); ./configure --enable-snuffleupagus
	make -C $(SRC)
	strip $(SRC)/.libs/snuffleupagus.so

install: release  ## compile and install snuffleupagus
	make -C $(SRC) install

compile_debug:  ## compile a debug build
	cd $(SRC); if [[ ! -f configure ]]; then phpize; fi; \
	./configure --enable-snuffleupagus --enable-debug --enable-debug-stderr && \
	make clean ; make -j4

DOCKER_IMAGE := php:latest
docker: ## start docker container with current PHP
	@echo "Starting new docker container with snuffleupagus bind-mounted to /sp"
	docker run -it -v "$$(pwd)":/sp $(DOCKER_IMAGE) /bin/bash

linked-clone:
	@if [[ "$(CLONE)" == "" ]]; then echo -e "==> Please provide clone name, e.g.\n  make linked-clone CLONE=php8.1\n"; exit 1; fi
	@if [[ -d "src-$(CLONE)" ]]; then echo -e "==> Clone '$(CLONE)' already exists.\n"; exit 1; fi
	@echo "==> CREATING LINKED CLONE IN 'src-$(CLONE)' <=="
	mkdir "src-$(CLONE)"; cd "src-$(CLONE)"; \
	SRCDIR=../src; ln -s $$SRCDIR/*.[hc] $$SRCDIR/config.m4 $$SRCDIR/snuffleupagus.php $$SRCDIR/Makefile.frag $$SRCDIR/*.re .; \
	cp -r $$SRCDIR/tests .
	@echo -e "==> DONE. <==\nCompile a debug build with\n  make compile_debug SRC=src-$(CLONE)"

tests: release tests-incremental  ## compile a release build and run the testsuite

tests-incremental:  ## perform an incremental build and run the testsuite
	make -C $(SRC)
	TEST_PHP_ARGS='-q' REPORT_EXIT_STATUS=1 make -C $(SRC) test

coverage:  ## compile snuffleugpaus, and run the testsuite with coverage
	cd $(SRC); phpize
ifeq ($(CC),clang)
	cd $(SRC); CFLAGS="-fprofile-instr-generate -fcoverage-mapping" ./configure --enable-snuffleupagus
	make -C $(SRC)
	sed -i "s/\$$ext_params -d display_errors=0 -r/-d display_errors=0 -r/" $(SRC)/run-tests.php
	LLVM_PROFILE_FILE="sp_%p_%m.profraw" TEST_PHP_ARGS='-q' REPORT_EXIT_STATUS=1 make -C $(SRC) test
	llvm-profdata-4.0 merge ./$(SRC)/*.profraw -o ./$(SRC)/sp.profdata
	llvm-cov report -instr-profile=./$(SRC)/sp.profdata ./$(SRC)/modules/snuffleupagus.so
else
	cd $(SRC); ./configure --enable-snuffleupagus --enable-coverage
	make -C $(SRC)
	rm -Rf $(SRC)/COV.html
	sed -i "s/\$$ext_params -d display_errors=0 -r/-d display_errors=0 -r/" $(SRC)/run-tests.php
	TEST_PHP_ARGS='-q' REPORT_EXIT_STATUS=1 make -C $(SRC) test
	lcov --base-directory ./$(SRC) --directory ./$(SRC) -c -o ./$(SRC)/COV.info --rc lcov_branch_coverage=1
	lcov --remove $(SRC)/COV.info '/usr/*' --remove $(SRC)/COV.info '*tweetnacl.c' -o $(SRC)/COV.info --rc lcov_branch_coverage=1
	genhtml --show-details -o $(SRC)/COV.html ./$(SRC)/COV.info  --branch-coverage
endif

bench: joomla  ## run the benchmark

joomla:
	if [ ! -d "joomla-cms" ]; then \
		git clone --depth 1 https://github.com/joomla/joomla-cms.git >/dev/null; \
	fi
	cd joomla-cms; composer install >/dev/null 2>/dev/null
	echo "\nWith snuffleupagus:"
	cd joomla-cms; time php -d "extension=../$(SRC)/modules/snuffleupagus.so" -d "sp.configuration_file=../config/default.rules" libraries/vendor/phpunit/phpunit/phpunit --no-coverage >/dev/null
	echo "\nWithout snuffleupagus:"
	cd joomla-cms; time php libraries/vendor/phpunit/phpunit/phpunit --no-coverage >/dev/null

packages: debian  ## produce packages

debian:
	dpkg-buildpackage -i -us -uc -tc -I -rfakeroot

help:
	@awk -F ':|##' '/^[^\t].+?:.*?##/ { printf "\033[36m%-30s\033[0m %s\n", $$1, $$NF }' $(MAKEFILE_LIST) | sort

.PHONY: debian
