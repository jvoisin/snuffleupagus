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
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on return of the function 'a', because the function returned 'OBJECT', which matched a rule in %a/disabled_functions_ret_type_object.php on line 5