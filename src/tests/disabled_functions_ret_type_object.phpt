--TEST--
Disable functions check on `ret` by type matching (object).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_object.ini
--FILE--
<?php 
function a(){
	return new StdClass();
}
$var = a();
echo "fail";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'a' in %a/tests/disabled_functions_ret_type_object.php:5, because the function returned 'OBJECT', which matched a rule.
