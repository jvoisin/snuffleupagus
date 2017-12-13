--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_int.ini
--FILE--
<?php
function foobar($id) {
  echo $id."\n";
}
foobar(1);
foobar(42);
foobar(1337);
foobar(13374242);
foobar(0x2A);
foobar("10");
?>
--EXPECTF--
1
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'foobar' in %a/tests/disabled_functions_param_int.php:3 has been disabled, because its argument '$id' content (42) matched a rule.
