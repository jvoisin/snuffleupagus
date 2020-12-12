--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
$a = 1338;
function test(){
    echo strtoupper("id") . "\n";
}
echo "Value of a: $a\n";
test();

$a = 1337;
echo "Value of a: $a\n";
test();
?>
--EXPECTF--
Value of a: 1338
ID
Value of a: 1337

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_local_var.php on line 4
