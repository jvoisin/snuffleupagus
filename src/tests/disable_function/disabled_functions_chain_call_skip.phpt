--TEST--
Disable functions by matching the calltrace, with a superfluous function in between
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_chain_call_skip.ini
--FILE--
<?php 

function a() {
	echo "I'm in the `a` function!\n";
	b();
}
function b() {
	echo "I'm in the `b` function!\n";
	c();
}
function c() {
	echo "I'm in the `c` function!\n";
}

a();
?>
--EXPECTF--
I'm in the `a` function!
I'm in the `b` function!

Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'a>c' in %s/tests/disable_function/disabled_functions_chain_call_skip.php on line 12
I'm in the `c` function!
