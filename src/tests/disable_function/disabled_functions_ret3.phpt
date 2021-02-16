--TEST--
Disable functions check on `ret`.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
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

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'Bob::a', because the function returned '2', which matched a rule in %a/disabled_functions_ret3.php on line 9
