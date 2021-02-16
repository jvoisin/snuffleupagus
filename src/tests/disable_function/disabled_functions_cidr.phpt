--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--INI--
sp.configuration_file={PWD}/config/disabled_functions_cidr.ini
--FILE--
<?php
system("echo 42");
?>
--EXPECTF--
Fatal error: [snuffleupagus][127.0.0.1][disabled_function][drop] Aborted execution on call of the function 'system' in %a/disabled_functions_cidr.php on line 2