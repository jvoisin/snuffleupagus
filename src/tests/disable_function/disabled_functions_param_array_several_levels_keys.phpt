--TEST--
Disable functions - match on an array value buried in several levels
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_array.ini
--FILE--
<?php
function foo($arr) {
	 echo $arr["a"]."\n";
}
$a=Array("test2"=>Array("pof"=>"pif", "bar"=>Array("qwe"=>"bbb")), "a"=>"cccc");
foo($a);

$a=Array("test2"=>Array("bar"=>Array("lol"=>"aaa")), "a"=>"dddd");
foo($a);
?>
--EXPECTF--
cccc

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo', because its argument '$arr' content (ARRAY) matched the rule '5' in %a/disabled_functions_param_array_several_levels_keys.php on line 3