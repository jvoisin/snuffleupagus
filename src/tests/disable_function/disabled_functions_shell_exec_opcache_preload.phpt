--TEST--
Disable functions - shell_exec via opcache.preload
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (!extension_loaded("opcache")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 70400) print "skip"; ?>
--EXTENSIONS--
opcache
--XFAIL--
--INI--
sp.configuration_file={PWD}/config/disabled_functions_extra.ini
allow_url_include=1
opcache.enable=1
opcache.enable_cli=1
opcache.preload=data://text/plain,%3C%3Fphp+shell_exec%28%22ls%22%29%3B
opcache.preload_user=nobody
--FILE--
<?php
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'shell_exec', %a matched a rule in data%a line 1
