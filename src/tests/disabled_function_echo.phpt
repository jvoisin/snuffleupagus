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
qwerty
Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'echo' in %a/disabled_function_echo.php on line 3