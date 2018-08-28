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
?>
--EXPECT--
