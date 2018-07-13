--TEST--
Echo hooking
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_echo.ini
--FILE--
<?php 
function test($a) {
  print "$a";
}
echo "qwe";
test("rty");
test("oops");
?>
--CLEAN--
--EXPECTF--
qwerty[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'echo' in %a/tests/disabled_function_echo.php:3.
