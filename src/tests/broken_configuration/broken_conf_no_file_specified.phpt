--TEST--
Broken configuration - No configuration file specified
--INI--
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--FILE--
<?php echo "1\n"; ?>
--EXPECT--
Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.
