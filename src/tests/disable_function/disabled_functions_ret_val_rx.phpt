--TEST--
Disable functions ret val rx
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_retval_rx.ini
--FILE--
<?php 
echo str_repeat("fufu",1)."\n";
echo str_repeat("fufufu",1);
?>
--EXPECTF--
fufu

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'str_repeat', because the function returned 'fufufu', which matched a rule in %a/disabled_functions_ret_val_rx.php on line 3
