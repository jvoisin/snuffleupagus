--TEST--
Invalid configuration file for upload
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_nonexisting_script.ini
--FILE--
<?php
echo 1;
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] The `script` (./non_existing_script.sh) doesn't exist on line 1.
1
