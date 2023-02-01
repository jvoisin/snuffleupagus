--TEST--
Disable functions - Require
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_require.ini
--FILE--
<?php 
$dir = __DIR__;

// Just in case
@unlink($dir . '/test.bla');
@unlink($dir . '/test.meh');

file_put_contents($dir . '/test.bla', "BLA");
file_put_contents($dir . '/test.meh', "MEH");
require $dir . '/test.bla';
require $dir . '/test.meh';
echo "1337";
?>
--EXPECTF--
BLA
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'require', because its argument 'inclusion path' content (%a%2Ftest.meh) matched a rule in %a/disabled_functions_require.php on line %d
