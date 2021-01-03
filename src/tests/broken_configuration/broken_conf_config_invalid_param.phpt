--TEST--
Broken configuration with invalid parameter warning
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_config_invalid_param.ini
--FILE--
<?php
function foo($blah, $x = null, $y = null) {
  echo "ok";
}

foo("qwe");
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][config][log] It seems that you are filtering on a parameter '$qwe' of the function 'foo', but the parameter does not exists. in %s/tests/broken_configuration/broken_conf_config_invalid_param.php on line %d
ok
