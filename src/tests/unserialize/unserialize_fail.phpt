--TEST--
Unserialize fail
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize.ini
--FILE--
<?php 
var_dump(unserialize('s:1:"a";'));
var_dump(unserialize('s:1:"a";alyualskdufyhalkdjsfhalkjdhflaksjdfhlkasdhflkahdawkuerylksjdfhlkssjgdflaksjdhflkasjdf'));
var_dump(unserialize('s:1:"a";dslfjklfjfkjfdjffjfjads'));
var_dump(unserialize(1,2,3,4));
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][unserialize][drop] The serialized object is too small. in %a/unserialize_fail.php on line 2