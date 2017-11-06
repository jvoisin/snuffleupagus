--TEST--
Disable functions check on `ret` by type matching (object).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_object.ini
--XFAIL--
We're not implementing hooking on retval of user functions yet
--FILE--
<?php 
function a(){
	return new StdClass();
}
$var = a();
echo "fail";
?>
--EXPECTF--
