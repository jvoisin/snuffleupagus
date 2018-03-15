--TEST--
Disable functions check on `ret`.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret.ini
memory_limit=-1
--FILE--
<?php 
class Bob {
	function a() {
        echo("We're in function `a`.\n");
		return 2;
	}
}
$b = new Bob();
echo "`a` returned: " . $b->a() . ".\n";
echo("We're at the end of the execution.\n");
?>
--EXPECTF--
We're in function `a`.
[snuffleupagus][0.0.0.0][disabled_function][drop] The execution has been aborted in %a/disabled_functions_ret3.php:9, because the return value (2) of the function 'Bob::a' matched a rule.
--XFAIL--
Match on ret is broken for non-native functions :/
