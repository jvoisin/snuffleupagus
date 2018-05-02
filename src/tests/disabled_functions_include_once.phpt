--TEST--
Disable functions - include_once
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_include.ini
--FILE--
<?php 
$dir = __DIR__;
file_put_contents($dir . '/test.bla', "BLA\n");
file_put_contents($dir . '/test.sim', "MEH\n");
include_once $dir . '/test.bla';
include_once $dir . '/test.sim';
echo "1337\n";
?>
--EXPECTF--
BLA
[snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'include_once' in %a/disabled_functions_include_once.php:%d, because its argument 'inclusion path' content (%a/test.sim) matched a rule.
MEH
1337
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . '/test.bla');
unlink($dir . '/test.sim');
?>
