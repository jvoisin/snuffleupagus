--TEST--
Echo hooking
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_echo.ini
--FILE--
<?php 
function test() {
  print "3\n";
}
$abc = 1;
test();
$abc = 123;
test();
?>
--CLEAN--
--EXPECTF--
3
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'echo' in %a/tests/disabled_function_echo_local_var.php:3.
