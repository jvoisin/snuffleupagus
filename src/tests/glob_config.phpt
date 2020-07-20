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
Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'foo' in %a/glob_config.php on line 3
1

Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'bla' in %a/glob_config.php on line 6
2