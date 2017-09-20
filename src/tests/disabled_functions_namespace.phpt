--TEST--
Disable functions: namespaces support isn't implemented now
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_namespace.ini
--FILE--
<?php 
namespace my_super_namespace {
	function my_function() {
		echo "1\n";
	}
}
namespace my_second_namespace {
	function my_function() {
		echo "2\n";
	}
}
namespace {
	function my_function() {
		echo "3\n";
	}
\strcmp("1", "2");
\my_super_namespace\my_function();
\my_second_namespace\my_function();
my_function();
}
?>
--XFAIL--
--EXPECTF--
[snuffleupagus] The call to the function 'strcmp' in %a/tests/disabled_functions_namespace.php:%d has been disabled.
