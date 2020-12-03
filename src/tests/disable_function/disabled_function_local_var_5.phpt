--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
function test(){
    echo strtoupper("id") . "\n";
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
ID
Value of a:
object(stdClass)#1 (1) {
  ["zxc"]=>
  string(16) "not a good value"
}

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_local_var_5.php on line 3
