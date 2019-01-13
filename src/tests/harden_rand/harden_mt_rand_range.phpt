--TEST--
Harden mt_rand range
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/harden_rand.ini
--FILE--
<?php 
for ($i=0; $i <=100; $i++) {
  if (mt_rand(0,1) > 1) {
    echo "Failed\n";
  }
}

?>
--EXPECT--
