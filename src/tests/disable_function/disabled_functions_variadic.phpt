--TEST--
Disable functions - support for variadic functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
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
Warning: [snuffleupagus][0.0.0.0][disable_function][log] Snuffleupagus doesn't support variadic functions yet, sorry. Check https://github.com/jvoisin/snuffleupagus/issues/164 for details. in %a/disabled_functions_variadic.php on line %d

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo' in %a/disabled_functions_variadic.php on line %d
