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
[snuffleupagus][0.0.0.0][unserialize][drop] The serialized object is too small.
bool(false)
[snuffleupagus][0.0.0.0][unserialize][drop] Invalid HMAC for s:1:"a";alyualskdufyhalkdjsfh
NULL
[snuffleupagus][0.0.0.0][unserialize][drop] The serialized object is too small.
bool(false)

Warning: unserialize() expects at most 2 parameters, 4 given in %a/tests/unserialize_fail.php on line %d
bool(false)