--TEST--
Harden mt_rand
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/harden_rand.ini
--FILE--
<?php 
mt_srand(0);
$a = mt_rand(0,100)."\n";
$b = mt_rand(0,100)."\n";
mt_srand(0);
$c = mt_rand(0,100)."\n";
$d = mt_rand(0,100)."\n";

if ($a == $c && $b == $d)
	echo 'lose';
else
	echo 'win';
?>
--EXPECT--
win
