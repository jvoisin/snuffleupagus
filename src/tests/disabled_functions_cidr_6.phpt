--TEST--
Disable functions
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--ENV--
return <<<EOF
REMOTE_ADDR=2001:0db8:0000:0000:0000:ff00:0042:8328
EOF;
--INI--
sp.configuration_file={PWD}/config/disabled_functions_cidr.ini
--FILE--
<?php
strpos("a", "b");
printf(1337);
?>
--EXPECTF--
[snuffleupagus][2001:0db8:0000:0000:0000:ff00:0042:8328][disabled_function][drop] The call to the function 'strpos' in %a/tests/disabled_functions_cidr_6.php:2 has been disabled.
1337
