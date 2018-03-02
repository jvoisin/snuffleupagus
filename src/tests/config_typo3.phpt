--TEST--
Rules for Typo3
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../config/typo3.rules
--FILE--
<?php 
echo "ok"
?>
--EXPECT--
ok
