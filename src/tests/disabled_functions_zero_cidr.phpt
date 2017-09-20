--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--INI--
sp.configuration_file={PWD}/config/disabled_functions_zero_cidr.ini
--FILE--
<?php
system("echo 42");
printf("1337");
?>
--EXPECTF--
[snuffleupagus][127.0.0.1][disabled_function][drop] The call to the function 'system' in %a/tests/disabled_functions_zero_cidr.php:2 has been disabled.
1337
