--TEST--
Disabled user-created functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_user_functions.ini
--FILE--
<?php 
function my_super_function() {
	echo 1;
}
my_super_function();
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'my_super_function' in %a/tests/disabled_user_functions.php:3 has been disabled.
