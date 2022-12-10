--TEST--
Disable functions - Require (allow)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_require_allow.ini
--FILE--
<?php 
$dir = __DIR__;

// Just in case
unlink($dir . '/test.bla');
unlink($dir . '/test.meh');

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
