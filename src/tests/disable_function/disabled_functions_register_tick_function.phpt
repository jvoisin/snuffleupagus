--TEST--
Disable functions - Called with register_tick_function
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_user_functions.ini
--FILE--
<?php 
declare(ticks=1);
function my_super_function() {
	echo 'lose';
}
echo "1337\n";
register_tick_function('my_super_function');
?>
--EXPECTF--
1337

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'my_super_function' in %a/disabled_functions_register_tick_function.php on line 4