--TEST--
Disable functions - match on a super global
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_super_global_var.ini
--GET--
bla=test
--FILE--
<?php 
function test(){
    echo strtoupper($_GET['bla']) . "\n";
}
test();
$_GET['bla'] = 'test2';
test();
?>
--EXPECTF--
TEST

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_super_global_var.php on line 3
