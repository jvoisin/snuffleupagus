--TEST--
Sloppy comparison array_keys SEGFAULT
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/sloppy_comparison.ini
--FILE--
<?php
$toto = [
        "toto" => 1,
        "titi" => 2,
        "tata" => 3,
];
var_dump(array_keys($toto));
var_dump(array_keys($toto, 0, 0)); // This should return all the keys since "toto" == 0 , but SP should block it
?>
--EXPECT--
array(3) {
  [0]=>
  string(4) "toto"
  [1]=>
  string(4) "titi"
  [2]=>
  string(4) "tata"
}
array(0) {
}


