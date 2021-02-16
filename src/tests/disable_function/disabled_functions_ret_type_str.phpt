--TEST--
Disable functions check on `ret` by type matching (string).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_str.ini
--FILE--
<?php 
echo substr("pouet", 3) . "\n";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'substr', because the function returned 'et', which matched the rule 'Return value is a string' in %a/disabled_functions_ret_type_str.php on line 2