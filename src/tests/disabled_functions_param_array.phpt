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
$a=Array("a"=>"test1");
foo($a);
$a=Array("a"=>"abcd");
foo($a);
$a=Array("a"=>"abcde");
foo($a);
$a=Array("bla"=>"abcdef");
foo($a);
$a=Array("bla"=>"aaa", "a"=>"eee" );
foo($a);
$a=Array("test"=>"aaa", "a"=>"fff" );
foo($a);
$a=Array("test2"=>Array("foo"=>Array("lol"=>"bbb")), "a"=>"cccc");
foo($a);
$a=Array("test2"=>Array("foo"=>Array("lol"=>"aaa")), "a"=>"dddd");
foo($a);
?>
--EXPECTF--
test1
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'foo' in %a/disabled_functions_param_array.php:3 has been disabled, because its argument 'arr' content (Array) matched the rule '1'.
