--TEST--
Check the phplog output with a log_max_len
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/phplog_max_len.ini
--FILE--
<?php 
var_dump(ini_set("1234567890abcdefghijklmnopqrstuvwxyz", "value"));
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'ini_set', because its argument '$option' content (1234567890abcdef) matched a rule in %s/tests/phplog_max_len.php on line 2

