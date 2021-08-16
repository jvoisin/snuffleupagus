$(srcdir)/sp_config_scanner.c: $(srcdir)/sp_config_scanner.re
	@$(RE2C) $(RE2C_FLAGS) --no-generation-date -bc -o $@ $<
