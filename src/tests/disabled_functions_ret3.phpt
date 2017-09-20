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
		return 1;
	}
}
$b = new Bob();
echo "`a` returned: " . $b->a() . ".\n";
echo("We're at the end of the execution.\n");
?>
--EXPECTF--
We're in function `a`.
`a` returned: 1.
We're at the end of the execution.