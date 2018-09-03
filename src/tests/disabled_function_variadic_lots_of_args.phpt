--TEST--
Disable functions - match on a variadic param
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_variadic.ini
--FILE--
<?php
function test_variadic_lot_args($asd) {
  var_dump(func_get_args());
}

test_variadic_lot_args("lqwe");
test_variadic_lot_args("lqwe", array("alel"), "zxc", "asd", "qwe");
test_variadic_lot_args(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
test_variadic_lot_args(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, "dropthis", 13);
?>
--EXPECTF--
Warning: [snuffleupagus][config] It seems that you wrote a rule filtering on the 12th argument of the function 'test_variadic_lot_args', but it takes only 1 arguments. Matching on _all_ arguments instead. in %a/tests/disabled_function_variadic_lots_of_args.php on line %d
array(1) {
  [0]=>
  string(4) "lqwe"
}

Warning: [snuffleupagus][config] It seems that you wrote a rule filtering on the 12th argument of the function 'test_variadic_lot_args', but it takes only 5 arguments. Matching on _all_ arguments instead. in %a/tests/disabled_function_variadic_lots_of_args.php on line %d
array(5) {
  [0]=>
  string(4) "lqwe"
  [1]=>
  array(1) {
    [0]=>
    string(4) "alel"
  }
  [2]=>
  string(3) "zxc"
  [3]=>
  string(3) "asd"
  [4]=>
  string(3) "qwe"
}
array(13) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
  [6]=>
  int(6)
  [7]=>
  int(7)
  [8]=>
  int(8)
  [9]=>
  int(9)
  [10]=>
  int(10)
  [11]=>
  int(11)
  [12]=>
  int(12)
}

Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'test_variadic_lot_args', because its argument 'variadic' content (dropthis) matched a rule in %a/tests/disabled_function_variadic_lots_of_args.php on line %d
