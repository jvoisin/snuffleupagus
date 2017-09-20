--TEST--
Disable functions - Require
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_require.ini
--FILE--
<?php 
$dir = __DIR__;
file_put_contents($dir . '/test.meh', "");
file_put_contents($dir . '/test.bla', "");
require $dir . '/test.meh';
require $dir . '/test.bla';
echo "1337";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][include][drop] Inclusion of a forbidden file (%a/test.bla).
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . '/test.meh');
unlink($dir . '/test.bla');
?>
