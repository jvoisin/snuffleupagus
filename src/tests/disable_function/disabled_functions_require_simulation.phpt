--TEST--
Disable functions - Require (simulation)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_require.ini
--FILE--
<?php 
$dir = __DIR__;
// Just in case
@unlink($dir . '/test.bla');
@unlink($dir . '/test.sim');

file_put_contents($dir . '/test.bla', "BLA\n");
file_put_contents($dir . '/test.sim', "MEH\n");
require $dir . '/test.bla';
require $dir . '/test.sim';
echo "1337\n";
?>
--EXPECTF--
BLA

Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'require', because its argument 'inclusion path' content (%a/test.sim) matched a rule in %a/disabled_functions_require_simulation.php on line %d
MEH
1337
