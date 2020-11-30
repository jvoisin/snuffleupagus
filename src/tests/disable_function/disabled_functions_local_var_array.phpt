--TEST--
Disable functions - match on an array value buried in several levels
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_local_var_array.ini
--FILE--
<?php
function foo($arr) {
	 echo $arr["a"]."\n";
}
$a=Array("test2"=>Array("pof"=>"pif", "foo"=>Array("lol"=>"bbb")), "a"=>"cccc");
foo($a);

$a=Array("test2"=>Array("foo"=>Array("lol"=>"aaa")), "a"=>"dddd");
foo($a);
?>
--EXPECTF--
cccc

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo' in %a/disabled_functions_local_var_array.php on line 3