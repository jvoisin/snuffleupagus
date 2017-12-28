--TEST--
Disable functions - Called with register_shutdown_function
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_user_functions_die.ini
--FILE--
<?php 
function my_super_function() {
	echo 'lose';
}
echo 1337;
register_shutdown_function('my_super_function');
?>
--EXPECTF--
1337
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'die' in %a/tests/disabled_functions_register_shutdown_function.php:%d has been disabled.
--XFAIL--
