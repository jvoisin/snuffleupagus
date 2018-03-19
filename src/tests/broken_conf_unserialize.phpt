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
[snuffleupagus][0.0.0.0][config][error] Trailing chars '234);' at the end of '.enable(1234);' on line 1.
1
