--TEST--
Allow broken: a broken file discards rules from previously parsed files
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/allow_broken_good.ini,{PWD}/config/broken_conf.ini
sp.allow_broken_configuration=On
error_log=/dev/null
display_errors=On
--FILE--
<?php
system("echo 1337");
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][config][log] parser error in %s/config/broken_conf.ini:1 in %s/config/broken_conf.ini on line 1

Warning: [snuffleupagus][0.0.0.0][config][log] Invalid configuration; all Snuffleupagus protections are disabled in Unknown on line 0
1337
