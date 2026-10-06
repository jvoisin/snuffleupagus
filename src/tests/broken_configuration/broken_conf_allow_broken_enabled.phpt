--TEST--
Broken configuration with allow broken turned on
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf.ini
sp.allow_broken_configuration=On
error_log=/dev/null
display_errors=On
--FILE--
<?php
echo "1337\n";
trigger_error("runtime error", E_USER_WARNING);
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] parser error in %s/tests/broken_configuration/config/broken_conf.ini:1 in %s/broken_configuration/config/broken_conf.ini on line 1
1337

Warning: runtime error in %s/broken_conf_allow_broken_enabled.php on line 3
