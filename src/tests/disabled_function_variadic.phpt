--TEST--
Disable functions - match on a variadic param
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_variadic.ini
--FILE--
<?php
function test_variadic($asd) {
  var_dump(func_get_args());
}

test_variadic("lqwe");
test_variadic("lqwe", array("alel"), "dropthis", "asd", "qwe");
test_variadic("lqwe", "dropthis", "asd", "qwe");
?>
--EXPECTF--
Warning: [snuffleupagus][config] It seems that you wrote a rule filtering on the 1st argument of the function 'test_variadic', but it takes only 1 arguments. Matching on _all_ arguments instead. in %a/tests/disabled_function_variadic.php on line %d

Warning: [snuffleupagus][config] It seems that you wrote a rule filtering on the 321th argument of the function 'test_variadic', but it takes only 1 arguments. Matching on _all_ arguments instead. in %a/tests/disabled_function_variadic.php on line %d
array(1) {
  [0]=>
  string(4) "lqwe"
}

Warning: [snuffleupagus][config] It seems that you wrote a rule filtering on the 321th argument of the function 'test_variadic', but it takes only 5 arguments. Matching on _all_ arguments instead. in %a/tests/disabled_function_variadic.php on line %d
array(5) {
  [0]=>
  string(4) "lqwe"
  [1]=>
  array(1) {
    [0]=>
    string(4) "alel"
  }
  [2]=>
  string(8) "dropthis"
  [3]=>
  string(3) "asd"
  [4]=>
  string(3) "qwe"
}

Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'test_variadic', because its argument 'variadic' content (dropthis) matched a rule in %a/tests/disabled_function_variadic.php on line %d
