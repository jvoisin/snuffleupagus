--TEST--
Sloppy comparison array_keys
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/sloppy_comparison.ini
--FILE--
<?php 
$qwe = array(rand(1,2), "qwe");
var_dump(array_keys($qwe, 0));
var_dump(array_keys($qwe, 0, FALSE));
var_dump(array_keys($qwe, 0, TRUE));

$toto = [
        "toto" => 1,
        "titi" => 2,
        "tata" => 3,
];

var_dump(array_keys($toto));

?>
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}
array(3) {
  [0]=>
  string(4) "toto"
  [1]=>
  string(4) "titi"
  [2]=>
  string(4) "tata"
}
