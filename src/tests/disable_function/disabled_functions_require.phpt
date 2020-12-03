--TEST--
Disable functions - Require
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_require.ini
--FILE--
<?php 
$dir = __DIR__;
file_put_contents($dir . '/test.bla', "BLA");
file_put_contents($dir . '/test.meh', "MEH");
require $dir . '/test.bla';
require $dir . '/test.meh';
echo "1337";
?>
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . '/test.bla');
unlink($dir . '/test.meh');
?>
--EXPECTF--
BLA
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'require', because its argument 'inclusion path' content (%a/test.meh) matched a rule in %a/disabled_functions_require.php on line 6
