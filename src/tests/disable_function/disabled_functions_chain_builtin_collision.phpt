--TEST--
A chained rule ending on a builtin must not shadow a plain rule on that builtin
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_chain_builtin_collision.ini
--FILE--
<?php
echo "this should not be printed\n";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'echo' in %a/disabled_functions_chain_builtin_collision.php on line 2
