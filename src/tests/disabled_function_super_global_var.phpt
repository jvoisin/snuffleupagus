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
    echo strlen($_GET['bla']) . "\n";
}
test();
$_GET['bla'] = 'test2';
test();
?>
--EXPECTF--
4
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %s/tests/disabled_function_super_global_var.php:%d has been disabled.
