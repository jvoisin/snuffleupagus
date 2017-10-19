--TEST--
Multiple configuration files
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_multi2.ini,{PWD}/config/config_multi1.ini
--FILE--
<?php
function foo() {
echo "1\n";
}
function bla() {
echo "2\n";
}
foo();
bla();
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][simulation] The call to the function 'foo' in %s/src/tests/multi_config.php:%d has been disabled.
1
[snuffleupagus][0.0.0.0][disabled_function][simulation] The call to the function 'bla' in %s/src/tests/multi_config.php:%d has been disabled.
2

