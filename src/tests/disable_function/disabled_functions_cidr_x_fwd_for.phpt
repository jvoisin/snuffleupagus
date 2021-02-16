--TEST--
Disable functions - CIDR match on an x-forwarded-for
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--ENV--
return <<<EOF
HTTP_X_FORWARDED_FOR=127.0.0.1
EOF;
--INI--
sp.configuration_file={PWD}/config/disabled_functions_cidr.ini
--FILE--
<?php
system("echo 42");
?>
--EXPECTF--
Fatal error: [snuffleupagus][127.0.0.1][disabled_function][drop] Aborted execution on call of the function 'system' in %a/disabled_functions_cidr_x_fwd_for.php on line 2
