--TEST--
Unserialize ok
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize_sim.ini
--FILE--
<?php 
$a=serialize("a");
echo $a;
var_dump(unserialize($a));
var_dump(unserialize('s:1:"a";alyualskdufyhalkdjsfhalkjdhflaksjdfhlkasdhflkahdawkuerylksjdfhlkssjgdflaksjdh1337sjdf'));
?>
--EXPECTF--
s:1:"a";650609b417904d0d9bbf1fc44a975d13ecdf6b02b715c1a06271fb3b673f25b1string(1) "a"

Warning: [snuffleupagus][unserialize] Invalid HMAC for s:1:"a";alyualskdufyhalkdjsfh in %a/src/tests/unserialize_sim.php on line 5
string(1) "a"
