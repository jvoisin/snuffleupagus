--TEST--
Harden rand without any arguments
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/harden_rand.ini
We should fix this
--FILE--
<?php 
rand();
mt_rand();

rand(1);
mt_rand(1);

rand(1, 2);
mt_rand(1, 2);

rand(2, 1);
mt_rand(2, 1);

rand(2, 1, 0);
mt_rand(2, 1, 0);

rand("test", 1);
mt_rand("test", 1);

rand(1, "test");
mt_rand(1, "test");

rand(1, 2, "test");
mt_rand(1, 2, "test");

echo "Everything is fine\n";
echo "Absolutely everything\n";
echo 'Even with single quotes';
?>
--EXPECTF--
Warning: rand() expects exactly 2 parameters, 1 given in %s/harden_rand_noargs.php on line %d

Warning: mt_rand() expects exactly 2 parameters, 1 given in %s/harden_rand_noargs.php on line %d

Warning: mt_rand(): max(1) is smaller than min(2) in %s/harden_rand_noargs.php on line %d

Warning: rand() expects exactly 2 parameters, 3 given in %s/harden_rand_noargs.php on line %d

Warning: mt_rand() expects exactly 2 parameters, 3 given in %s/harden_rand_noargs.php on line %d

Warning: rand() expects parameter 1 to be in%s, string given in %s/harden_rand_noargs.php on line %d

Warning: mt_rand() expects parameter 1 to be in%s, string given in %s/harden_rand_noargs.php on line %d

Warning: rand() expects parameter 2 to be in%s, string given in %s/harden_rand_noargs.php on line %d

Warning: mt_rand() expects parameter 2 to be in%s, string given in %s/harden_rand_noargs.php on line %d

Warning: rand() expects exactly 2 parameters, 3 given in %s/harden_rand_noargs.php on line %d

Warning: mt_rand() expects exactly 2 parameters, 3 given in %s/harden_rand_noargs.php on line %d
Everything is fine
Absolutely everything
Even with single quotes
