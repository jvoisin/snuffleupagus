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
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'my_func' in %a/disabled_functions_eval_user.php on line 3