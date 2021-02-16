--TEST--
Disable functions check on `ret` with an alias
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_dump_segfault1.ini
--FILE--
<?php 
echo strpos("pouet", "p") . "\n";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'strpos', because the function returned '0', which matched the rule 'test' in %a/dump_segfault1.php on line 2