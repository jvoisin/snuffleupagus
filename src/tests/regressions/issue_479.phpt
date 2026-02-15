--TEST--
Regression test for https://github.com/jvoisin/snuffleupagus/issues/479
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
<?php if (!extension_loaded("simplexml")) print("skip Required extension missing: simplexml"); ?>
--INI--
sp.configuration_file={PWD}/../config/issue_479.ini
--FILE--
<?php
echo 1+1;
?>
--EXPECTF--
2
