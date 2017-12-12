--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_array.ini
--FILE--
<?php
function foo($arr) {
	 echo $arr["a"]."\n";
}

$a=Array("bla"=>"aaa", "a"=>"eee" );
foo($a);

$a=Array("bla"=>"aaa", "a"=>"abcdef" );
foo($a);

$a=Array("bla"=>"abcdef", "not_bla"=>"134");
foo($a);
?>
--EXPECTF--
eee
abcdef
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'foo' in %a/tests/disabled_functions_param_array_deref.php:3 has been disabled, because its argument 'arr' content (abcdef) matched the rule '2'.
