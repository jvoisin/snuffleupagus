--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
function test(){
    echo strlen("id") . "\n";
}

$a = Array();
echo "Value of a:\n";
var_dump($a);
test();

$a = (Object)['zxc' => 'not a good value'];
echo "Value of a:\n";
var_dump($a);
test();
?>
--EXPECTF--
Value of a:
array(0) {
}
2
Value of a:
object(stdClass)#1 (1) {
  ["zxc"]=>
  string(16) "not a good value"
}
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/tests/disabled_function_local_var_5.php:%d has been disabled.
