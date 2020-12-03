--TEST--
Disable functions - Require (allow)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_require_allow.ini
--FILE--
<?php 
$dir = __DIR__;
file_put_contents($dir . '/test.bla', "BLA\n");
file_put_contents($dir . '/test.meh', "MEH\n");
require $dir . '/test.bla';
require $dir . '/test.meh';
echo "1337";
?>
--EXPECTF--
BLA
MEH
1337
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . '/test.bla');
unlink($dir . '/test.meh');
?>
