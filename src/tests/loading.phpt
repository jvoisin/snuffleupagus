--TEST--
Check for snuffleupagus presence
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--FILE--
<?php 
echo "snuffleupagus extension is available";
?>
--EXPECT--
snuffleupagus extension is available
