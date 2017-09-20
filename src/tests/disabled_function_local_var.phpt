--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
$a = 1338;
function test(){
    echo strlen("id") . "\n";
}
echo "Value of a: $a\n";
test();

$a = 1337;
echo "Value of a: $a\n";
test();
?>
--EXPECTF--
Value of a: 1338
2
Value of a: 1337
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/tests/disabled_function_local_var.php:%d has been disabled.