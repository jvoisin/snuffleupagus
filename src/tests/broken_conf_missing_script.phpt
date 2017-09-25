--TEST--
Invalid configuration file for upload
--INI--
file_uploads=1
sp.configuration_file={PWD}/config/broken_conf_missing_script.ini
--FILE--
<?php
echo 1;
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] The `script` directive is mandatory in '.enable();' on line 1.
1
