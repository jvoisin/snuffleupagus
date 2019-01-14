--TEST--
Invalid client IP
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--ENV--
return <<<EOF
REMOTE_ADDR=xyz
EOF;
--INI--
sp.configuration_file={PWD}/config/disabled_functions_cidr.ini
--FILE--
<?php
strpos("1337", "1");
?>
--EXPECTF--
Fatal error: [snuffleupagus][cidr_match] Weird ip (xyz) family in %a/broken_invalid_client_ip4.php on line 2