--TEST--
Global strict mode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip Not applicable for PHP 8+"; ?>
--INI--
sp.configuration_file={PWD}/config/global_strict_disabled.ini
--FILE--
<?php 
strcmp("pouet", []);
echo 1337;
?>
--EXPECTF--
Warning: strcmp() expects parameter 2 to be string, array given in %a/global_strict_disabled.php on line 2
1337
