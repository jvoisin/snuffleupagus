--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_name_type_php8.ini
--FILE--
<?php 
echo strcmp("pouet", "pouet") . "\n";
echo strcmp([1,23], "pouet") . "\n";
?>
--EXPECTF--
0

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strcmp', because its argument '$string1' content (?) matched a rule in %s/disabled_functions_name_type_php8.php on line 3
