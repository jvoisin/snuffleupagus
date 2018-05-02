--TEST--
Disable functions check on `ret` with an alias
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_dump_segfault1.ini
--FILE--
<?php 
echo strpos("pouet", "p") . "\n";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The return to the function 'strpos' in %a/tests/dump_segfault1.php:2 has been disabled, because the function returned '0', which matched the rule 'test'.

