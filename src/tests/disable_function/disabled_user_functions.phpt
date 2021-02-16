--TEST--
Disabled user-created functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
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
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'my_super_function' in %a/disabled_user_functions.php on line 3