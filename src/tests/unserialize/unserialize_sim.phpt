--TEST--
Unserialize ok
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize_sim.ini
--FILE--
<?php 
$a=serialize("a");
echo $a . PHP_EOL;
var_dump(unserialize($a));
var_dump(unserialize('s:1:"a";alyualskdufyhalkdjsfhalkjdhflaksjdfhlkasdhflkahdawkuerylksjdfhlkssjgdflaksjdh1337sjdf'));
?>
--EXPECTF--
s:1:"a";cdbc93e593656164d448db33e4668a3f30fa794d6658016365f7eb453d48b022
string(1) "a"

Warning: [snuffleupagus][0.0.0.0][unserialize][simulation] Invalid HMAC for s:1:"a";alyualskdufyhalkdjsfh in %a/unserialize_sim.php on line 5
string(1) "a"
