--TEST--
Disable functions - support for variadic functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_variadic.ini
--FILE--
<?php 
function foo(...$b) {
	return count($b);
}

echo foo(5, 4, 3, 2, 1);
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][disable_function] Snuffleupagus doesn't support variadic functions yet, sorry. Check https://github.com/nbs-system/snuffleupagus/issues/164 for details. in %a/disabled_functions_variadic.php on line %d

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'foo' in %a/disabled_functions_variadic.php on line %d
