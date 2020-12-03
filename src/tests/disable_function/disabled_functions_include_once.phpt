--TEST--
Disable functions - include_once
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
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
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . '/test.bla');
unlink($dir . '/test.sim');
?>
--EXPECTF--
BLA

Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'include_once', because its argument 'inclusion path' content (%a/test.sim) matched a rule in %a/disabled_functions_include_once.php on line 6
MEH
1337
