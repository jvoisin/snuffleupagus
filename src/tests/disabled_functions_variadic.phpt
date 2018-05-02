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
[snuffleupagus][0.0.0.0][disable_function][error] Snuffleupagus doesn't support variadic functions yet, sorry. Check https://github.com/nbs-system/snuffleupagus/issues/164 for details.
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo' in %a/disabled_functions_variadic.php:2.
