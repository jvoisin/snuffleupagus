--TEST--
Disable functions by matching the calltrace, on the return value
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_chain_call_user_func_ret.ini
--FILE--
<?php 
function two($a) {
	echo "two\n";
	return $a . "_two";
}
function one($a) {
	echo "one\n";
	two($a);
	return $a . "_one";
}

echo one('not matching') . "\n";
echo one('matching') . "\n";
echo one('still not matching') . "\n";

?>
--XFAIL--
--EXPECTF--
Match on ret is broken :/


Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0
Could not startup.