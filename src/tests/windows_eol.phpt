--TEST--
Windows EOL in configuration file
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/windows_eol.ini
--FILE--
<?php echo "1\n"; ?>
--EXPECT--
1
