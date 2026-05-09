--TEST--
Echo hooking
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_echo.ini
opcache.optimization_level=0
--FILE--
<?php 
echo "qwe";
echo "1";
echo "oops";
?>
--EXPECTF--
qwe1
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'echo' in %a/disabled_function_echo_2.php on line 4
