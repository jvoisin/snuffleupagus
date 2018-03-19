--TEST--
Harden rand
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/harden_rand.ini
--FILE--
<?php 
srand(0);
$a = rand(0,1000)."\n";
$b = rand(0,1000)."\n";
srand(0);
$c = rand(0,1000)."\n";
$d = rand(0,1000)."\n";

rand(100,0)."\n";

if ($a == $c && $b == $d)
    echo "a:$a, b:$b, c:$c, d:$d\n";
else
	echo 'win';
?>
--EXPECT--
win
