--TEST--
Disable functions check on `ret` by type matching (array).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_array.ini
--FILE--
<?php 
echo get_loaded_extensions();
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'get_loaded_extensions', because the function returned 'ARRAY', which matched the rule 'Return value is an array' in %a/disabled_functions_ret_type_array.php on line 2