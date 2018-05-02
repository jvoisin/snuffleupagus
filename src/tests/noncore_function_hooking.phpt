--TEST--
Hooking of user-defined functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_noncore_function_hooking.ini
--FILE--
<?php 
function custom_fun($a) {
	echo $a;
}
custom_fun("hello");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'custom_fun' in %a/tests/noncore_function_hooking.php:3.
