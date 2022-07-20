$(srcdir)/sp_config_scanner.c: $(srcdir)/sp_config_scanner.re
	if re2c -v |grep ' [23]\.' 2>/dev/null; then \
		re2c -bc -o $@ $<; \
		re2c --no-generation-date --no-version -bci -o $(srcdir)/sp_config_scanner.cached.c $<; \
	else \
		cp $(srcdir)/sp_config_scanner.cached.c $@; \
	fi;

.PHONY: tests
tests:
	$(MAKE) test NO_INTERACTION=1 SP_NODEBUG=1 SP_SKIP_OLD_PHP_CHECK=1 $(TESTS)
