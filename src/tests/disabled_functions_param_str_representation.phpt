--TEST--
Disable functions - casting various types to string internally
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_str_representation.ini
--FILE--
<?php 
echo var_export(true) . "\n";
echo var_export(false) . "\n";
echo var_export(null) . "\n";
echo var_export(1) . "\n";
echo var_export(1.0) . "\n";
function f(&$a) {
	echo var_export($a) . "\n";
}
$a = 123; f($a);
?>
--EXPECTF--
true
false
NULL
1
1.0
123
