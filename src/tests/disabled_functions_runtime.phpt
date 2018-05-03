--TEST--
Disable functions - runtime inclusion
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_param_runtime.ini
--FILE--
<?php 

$dir = __DIR__;
$content = '<?php function test($param) { echo $param . "\n"; }';
file_put_contents('file_to_include1.php', $content);
file_put_contents('file_to_include2.php', $content);

if (rand() % 2) {
    include "file_to_include1.php";
} else {
    include "file_to_include2.php";
}

test('1338');test('1337');

?>
--EXPECTF--
1338
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'test' in %a, because its argument '$param' content (1337) matched a rule.
--CLEAN--
<?php
unlink("file_to_include1.php");
unlink("file_to_include2.php");
?>
