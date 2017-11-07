--TEST--
Disable functions - echo
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_echo.ini
--FILE--
<?php
$a = 123;
echo "Value of a: $a\n";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'echo' in %a/tests/disabled_functions_echo.php:%d has been disabled.
