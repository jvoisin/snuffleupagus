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
echo foo(5, 4, 2, 2, 1);
?>
--EXPECTF--
5
Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'foo', because its argument 'variadic' content (2) matched a rule in %a/tests/disabled_functions_variadic.php on line %d
