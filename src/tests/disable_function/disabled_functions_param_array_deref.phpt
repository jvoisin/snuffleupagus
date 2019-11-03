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

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'foo', because its argument '$arr' content (abcdef) matched the rule '2' in %a/disabled_functions_param_array_deref.php on line 3