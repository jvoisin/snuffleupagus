--TEST--
Disable functions - match on an array value buried in several levels
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_local_var_array_key.ini
--FILE--
<?php
function foo($arr) {
	 echo $arr["a"]."\n";
}
$a=Array("test2"=>Array("pof"=>"pif", "foo"=>Array("lol"=>"bbb")), "a"=>"cccc");
foo($a);

$a=Array("test2"=>Array("foo"=>Array("aaa"=>"ccc")), "a"=>"dddd");
foo($a);
?>
--EXPECTF--
cccc

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo' in %a/disabled_functions_local_var_array_key.php on line 3
