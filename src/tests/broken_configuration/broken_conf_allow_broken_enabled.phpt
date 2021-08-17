--TEST--
Broken configuration with allow broken turned on
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf.ini
sp.allow_broken_configuration=On
--FILE--
<?php
echo 1337;
?>
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][config][log] Parser error on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Parser error on line 1 in Unknown on line 0
1337
