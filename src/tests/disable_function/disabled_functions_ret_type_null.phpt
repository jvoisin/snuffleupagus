--TEST--
Disable functions check on `ret` by type matching (null).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_null.ini
--FILE--
<?php 
function my_function() {
	return NULL;
}

var_dump(my_function());
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on return of the function 'my_function', because the function returned 'NULL', which matched the rule 'Return value is null' in %a/disabled_functions_ret_type_null.php on line 6