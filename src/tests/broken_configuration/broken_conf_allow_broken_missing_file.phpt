--TEST--
Allow broken: a missing configuration file doesn't abort startup
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/does_not_exist.ini
sp.allow_broken_configuration=On
error_log=/dev/null
display_errors=On
--FILE--
<?php
echo "1337\n";
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][config][log] Could not open configuration file %s/does_not_exist.ini : %s in %s/does_not_exist.ini on line 0

Warning: [snuffleupagus][0.0.0.0][config][log] Invalid configuration; all Snuffleupagus protections are disabled in Unknown on line 0
1337
