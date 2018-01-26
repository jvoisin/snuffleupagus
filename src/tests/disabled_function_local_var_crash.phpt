--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
function test(){
//    $a = "1337";
    echo strlen("id") . "\n";
}
ob_start(test);
echo "lol\n";
?>
--EXPECTF--
Notice: Use of undefined constant test - assumed 'test' in %a/disabled_function_local_var_crash.php on line %d
