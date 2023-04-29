--TEST--
Broken configuration with allow broken turned on
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf.ini
sp.allow_broken_configuration=On
error_log=/dev/null
--FILE--
<?php
echo 1337;
?>
--EXPECT--
Fatal error: [snuffleupagus][0.0.0.0][config][log] parser error on line 1 in Unknown on line 0
1337
