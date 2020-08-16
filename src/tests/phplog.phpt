--TEST--
Check the phplog output
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/phplog.ini
--FILE--
<?php 
var_dump(unserialize('s:1:"a";'));
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][unserialize][drop] The serialized object is too small. in %s/tests/phplog.php on line 2

