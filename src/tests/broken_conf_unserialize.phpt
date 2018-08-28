--TEST--
Invalid configuration file for unserialize
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_unserialize.ini
--FILE--
<?php
echo 1;
?>
--EXPECTF--
PHP Fatal error:  [snuffleupagus][config] Trailing chars '234);' at the end of '.enable(1234);' on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][config] Trailing chars '234);' at the end of '.enable(1234);' on line 1 in Unknown on line 0
1