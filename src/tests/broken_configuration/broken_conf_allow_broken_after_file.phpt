--TEST--
Allow broken: the flag is honoured when set after sp.configuration_file in the INI
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf.ini
error_log=/dev/null
display_errors=On
sp.allow_broken_configuration=On
--FILE--
<?php
echo "1337\n";
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][config][log] parser error in %s/config/broken_conf.ini:1 in %s/config/broken_conf.ini on line 1

Warning: [snuffleupagus][0.0.0.0][config][log] Invalid configuration; all Snuffleupagus protections are disabled in Unknown on line 0
1337
