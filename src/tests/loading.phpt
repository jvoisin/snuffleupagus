--TEST--
Check for snuffleupagus presence
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--FILE--
<?php 
echo "snuffleupagus extension is available";
?>
--EXPECT--
Warning: [snuffleupagus][0.0.0.0][config][log] No configuration specificed via sp.configuration_file in Unknown on line 0
snuffleupagus extension is available
