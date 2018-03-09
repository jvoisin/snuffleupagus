--TEST--
Disable functions - eval with a disabled user func
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_eval_user_func.ini
--FILE--
<?php 
function my_func() {
	echo "1337\n";
	return 10;
}
$a = 0;
eval('$a = my_func();');
echo '$a = ' . $a . "\n";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'my_func' in %a/tests/disabled_functions_eval_user.php:3 has been disabled.
