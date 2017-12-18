--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php
$qwe = Array('123' => Array('qwe'), '456' => Array('no block this'));
var_dump($qwe);
strlen("qwe");
$qwe = Array('123' => Array('qwe'), '456' => Array(Array('block this')));
var_dump($qwe);
strlen("qwe");
?>
--EXPECTF--
array(2) {
  [123]=>
  array(1) {
    [0]=>
    string(3) "qwe"
  }
  [456]=>
  array(1) {
    [0]=>
    string(13) "no block this"
  }
}
array(2) {
  [123]=>
  array(1) {
    [0]=>
    string(3) "qwe"
  }
  [456]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      string(10) "block this"
    }
  }
}
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in /home/slefevre/snuffleupagus/src/tests/disabled_function_local_var_10.php:7 has been disabled.
