--TEST--
Rules for SPIP
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../../config/spip.rules
--FILE--
<?php
echo "ok";
?>
--EXPECT--
ok
