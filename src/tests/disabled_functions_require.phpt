--TEST--
Disable functions - Require
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
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
Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'require', because its argument 'inclusion path' content (%a/test.meh) matched a rule in %a/disabled_functions_require.php on line 6
