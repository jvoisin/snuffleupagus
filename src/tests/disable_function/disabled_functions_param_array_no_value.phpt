--TEST--
Disable functions - matching on an array's variable only
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_array.ini
--FILE--
<?php
function foo($arr) {
	 echo $arr["a"]."\n";
}

$a=Array("test2"=>Array("foo"=>Array("lol"=>"bbb")), "a"=>"cccc");
foo($a);

$a=Array("test"=>"aaa", "a"=>"fff" );
foo($a);

?>
--EXPECTF--
cccc

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo', because its argument '$arr' content (aaa) matched the rule '3' in %a/disabled_functions_param_array_no_value.php on line 3