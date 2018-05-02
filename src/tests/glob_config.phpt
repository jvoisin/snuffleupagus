--TEST--
Multiple configuration files
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_multi*.ini
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
[snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'foo' in %s/tests/glob_config.php:%d.
1
[snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'bla' in %s/tests/glob_config.php:%d.
2

