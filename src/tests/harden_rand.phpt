--TEST--
Harden rand
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/harden_rand.ini
--FILE--
<?php 
srand(0);
$a = rand(0,100)."\n";
$b = rand(0,100)."\n";
srand(0);
$c = rand(0,100)."\n";
$d = rand(0,100)."\n";

rand(100,0)."\n";

if ($a == $c && $b == $d)
	echo 'fail';
else
	echo 'win';
?>
--EXPECT--
win
