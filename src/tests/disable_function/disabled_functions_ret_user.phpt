--TEST--
Check NULL return value for user func
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_ret_user.ini
--FILE--
<?php 
function qwe() {
  return "asd";
}
qwe();
echo 1;
?>
--EXPECTF--
Fatal error: [snuffleupagus][disabled_function] Aborted execution on return of the function 'qwe', because the function returned 'asd', which matched a rule in %a/disabled_functions_ret_user.php on line %d
