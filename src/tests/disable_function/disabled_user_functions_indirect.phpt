--TEST--
Disabled user-created functions, called indirectly
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_user_functions.ini
--FILE--
<?php 
function my_super_function() {
	echo 1;
}
array_map('my_super_function', [1,2,3,4]);
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'my_super_function' in %a/disabled_user_functions_indirect.php on line 3