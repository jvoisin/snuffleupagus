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
--EXPECTF--
BLA[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'include' in %a/disabled_functions_require.php:%d has been disabled, because its argument 'inclusion path' content (%a/test.meh) matched a rule.
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . '/test.bla');
unlink($dir . '/test.meh');
?>
