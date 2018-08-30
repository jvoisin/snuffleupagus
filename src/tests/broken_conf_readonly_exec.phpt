--TEST--
Invalid configuration file for readonly_exec
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_readonly_exec.ini
--FILE--
<?php
echo 1;
?>
--EXPECTF--
PHP Fatal error:  [snuffleupagus][config] Trailing chars '234);' at the end of '.enable(1234);' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Trailing chars '234);' at the end of '.enable(1234);' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.
