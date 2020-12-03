--TEST--
Rules for Typo3
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/../../config/typo3.rules
--FILE--
<?php 
echo "ok"
?>
--EXPECT--
ok
