--TEST--
Harden rand
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_rand_harden_disabled.ini
--FILE--
<?php 
srand(0);
echo rand(0,100)."\n";
srand(0);
echo rand(0,100)."\n";
?>
--EXPECT--
84
84
