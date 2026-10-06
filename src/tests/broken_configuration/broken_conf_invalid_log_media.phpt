--TEST--
Broken configuration filename with improper log media
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_invalid_log_media.ini
error_log=/dev/null
--FILE--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][config][log] .log_media() only supports 'syslog', 'file:' or 'php' in %s/tests/broken_configuration/config/broken_conf_invalid_log_media.ini:1, got 'pouet' instead in %s/broken_configuration/config/broken_conf_invalid_log_media.ini on line 1

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in %s/broken_configuration/config/broken_conf_invalid_log_media.ini on line 1
Could not startup.
