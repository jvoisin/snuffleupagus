--TEST--
Disable functions ret val - dump
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_retval_dump.ini
--ENV--
DOCUMENT_ROOT=a
--POST--
post_a=data_post_a&post_b=data_post_b
--GET--
get_a=data_get_a&get_b=data_get_b
--COOKIE--
cookie_a=data_cookie_a&cookie_b=data_cookie_b
--FILE--
<?php 
echo str_repeat("fufu",1)."\n";
echo str_repeat("fufufu",1);
?>
--EXPECTF--
fufu
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'str_repeat' in %a/disabled_functions_ret_val_dump.php:%d, because the function returned 'fufufu', which matched a rule.
