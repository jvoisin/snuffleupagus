--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_method.ini
--FILE--
<?php 
class AwesomeClass {
	function method1($a) {
		echo "method1:" . $a . "\n";
	}
	function method2($a) {
		echo "method2:" . $a . "\n";
	}
	function method3($a) {
		echo "method3:" . $a . "\n";
	}
}

$c = new AwesomeClass();
$c->method1("pif");
$c->method2("paf");
$c->method3("pouet");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'AwesomeClass::method1' in %a/tests/disabled_functions_method.php:4 has been disabled.
method2:paf
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'AwesomeClass::method3' in %a/tests/disabled_functions_method.php:10 has been disabled, because its argument 'a' content (pouet) matched a rule.
