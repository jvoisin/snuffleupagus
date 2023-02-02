--TEST--
Disable functions - include_once
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_include.ini
--FILE--
<?php 
$dir = __DIR__;

// Just inc ase
@unlink($dir . '/test.bla');
@unlink($dir . '/test.sim');

file_put_contents($dir . '/test.bla', "BLA\n");
file_put_contents($dir . '/test.sim', "MEH\n");
include_once $dir . '/test.bla';
include_once $dir . '/test.sim';
echo "1337\n";
?>
--EXPECTF--
BLA

Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'include_once', because its argument 'inclusion path' content (%a%2Ftest.sim) matched a rule in %a/disabled_functions_include_once.php on line %d
MEH
1337
