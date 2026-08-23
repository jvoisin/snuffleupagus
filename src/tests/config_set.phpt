--TEST--
Config scanner - @set variable definition and redefinition
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_set.ini
--FILE--
<?php
function foo() { echo "foo called\n"; }
function notafunc() { echo "notafunc called\n"; }
notafunc(); /* old @set value: replaced, so not disabled */
foo();      /* current @set value: disabled */
?>
--EXPECTF--
notafunc called

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo' in %a/config_set.php on line %d
