--TEST--
Broken configuration - No configuration file specified
--INI--
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--FILE--
<?php echo "1\n"; ?>
--EXPECT--
1
