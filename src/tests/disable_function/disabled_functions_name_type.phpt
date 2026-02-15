--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip Not applicable for PHP 8+"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_name_type.ini
--FILE--
<?php 
echo strcmp("pouet", "pouet") . "\n";
echo strcmp([1,23], "pouet") . "\n";
?>
--EXPECTF--
0

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strcmp', because its argument '$str1'%smatched a rule in %s/disabled_functions_name_type.php on line 3
