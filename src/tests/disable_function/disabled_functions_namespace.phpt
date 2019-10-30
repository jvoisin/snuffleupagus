--TEST--
Disable functions in namespaces
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_namespace.ini
--FILE--
<?php 
namespace my_super_namespace {
	function my_function() {
		echo "Should not be printed\n";
	}
}
namespace my_second_namespace {
	function my_function() {
		echo "Second namespace\n";
	}
}
namespace {
	function my_function() {
		echo "Anonymous namespace\n";
	}
\strcmp("1", "2");
\my_super_namespace\my_function();
\my_second_namespace\my_function();
my_function();
}
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'strcmp' in %a/disabled_functions_namespace.php on line 16