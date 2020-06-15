--TEST--
Echo hooking
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_print.ini
--FILE--
<?php 
function test($a) {
  print "$a";
}
print "qwe";
test("rty");
test("oops");
?>
--CLEAN--
--EXPECTF--
qwerty
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'echo' in %a/disabled_function_print.php on line 3