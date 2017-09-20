--TEST--
Harden rand
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_rand_harden_disabled.ini
--FILE--
<?php 
srand(0);
$a =  rand(0,100)."\n";
srand(0);
$b = rand(0,100)."\n";
srand(0);
$c = rand(0,100)."\n";
if ($a == $b && $a == $c) {
    echo "win";
} else {
    echo "lose";
}
?>
--EXPECT--
win
