--TEST--
Broken configuration - No configuration file specified
--INI--
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--FILE--
<?php echo "1\n"; ?>
--EXPECT--
Warning: [snuffleupagus][0.0.0.0][config][log] No configuration specificed via sp.configuration_file in Unknown on line 0
1
