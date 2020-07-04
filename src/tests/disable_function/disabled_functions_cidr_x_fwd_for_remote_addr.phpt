--TEST--
Disable functions - x-forwarded-for and remote-addr
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--ENV--
return <<<EOF
HTTP_X_FORWARDED_FOR=127.0.0.1
REMOTE_ADDR=127.0.0.2
EOF;
--INI--
sp.configuration_file={PWD}/config/disabled_functions_cidr.ini
--FILE--
<?php
system("echo 42");
?>
--EXPECTF--
Fatal error: [snuffleupagus][127.0.0.2][disabled_function] Aborted execution on call of the function 'system' in %a/disabled_functions_cidr_x_fwd_for_remote_addr.php on line 2
