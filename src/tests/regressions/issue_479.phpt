--TEST--
Regression test for https://github.com/jvoisin/snuffleupagus/issues/479
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || !extension_loaded("simplexml")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/../config/issue_479.ini
--FILE--
<?php
echo 1+1;
?>
--EXPECTF--
2
