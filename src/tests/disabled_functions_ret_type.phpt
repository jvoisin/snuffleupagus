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
[snuffleupagus][0.0.0.0][disabled_function][drop] The execution has been aborted in %a/disabled_functions_ret_type.php:%d, because the function 'strpos' returned 'FALSE', which matched the rule 'Return value is FALSE'.
