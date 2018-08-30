--TEST--
Check for snuffleupagus presence
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--FILE--
<?php 
echo "snuffleupagus extension is available";
?>
--EXPECT--
Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.
