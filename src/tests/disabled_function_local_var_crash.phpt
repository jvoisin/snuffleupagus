--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
function test(){
    $a = "1337";
    echo strlen("id") . "\n";
}
ob_start(test);
?>
--EXPECTF--
Notice: Use of undefined constant test - assumed 'test' in %a/disabled_function_local_var_crash.php on line %d
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/disabled_function_local_var_crash.php:%d has been disabled.
