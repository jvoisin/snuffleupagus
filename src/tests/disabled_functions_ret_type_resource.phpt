--TEST--
Disable functions check on `ret` by type matching (resource).
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type_resource.ini
--FILE--
<?php 
echo fopen("/etc/passwd", "r");
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'fopen' in %a/disabled_functions_ret_type_resource.php:2, because the function returned 'RESOURCE', which matched the rule 'Return value is a resource'.
