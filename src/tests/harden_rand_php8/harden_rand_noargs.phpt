--TEST--
Harden rand without any arguments
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/harden_rand.ini
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
Fatal error: Uncaught ArgumentCountError: rand() expects exactly 2 arguments, 1 given in %s/tests/harden_rand_php8/harden_rand_noargs.php:5
Stack trace:
#0 %s/tests/harden_rand_php8/harden_rand_noargs.php(5): rand(1)
#1 {main}
  thrown in %s/tests/harden_rand_php8/harden_rand_noargs.php on line 5
