--TEST--
Global strict mode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/global_strict.ini
--FILE--
<?php 
strcmp("pouet", []);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: strcmp() expects parameter 2 to be string, array given in %a/global_strict.php:2
Stack trace:
#0 %a/global_strict.php(2): strcmp('pouet', Array)
#1 {main}
  thrown in %a/global_strict.php on line 2
