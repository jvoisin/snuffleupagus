--TEST--
Disable functions check on `ret` by type matching (false)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret_type.ini
--FILE--
<?php 
var_dump(strpos("pouet", "p")) . "\n";
echo "1337\n";
echo strpos("pouet", "123");
?>
--EXPECTF--
int(0)
1337

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on return of the function 'strpos', because the function returned 'FALSE', which matched the rule 'Return value is FALSE' in %a/disabled_functions_ret_type.php on line 4