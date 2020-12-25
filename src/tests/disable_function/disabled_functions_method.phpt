--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
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
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'AwesomeClass::method1' in %a/disabled_functions_method.php on line 4
