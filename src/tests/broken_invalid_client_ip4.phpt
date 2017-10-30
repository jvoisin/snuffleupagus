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
[snuffleupagus][xyz][cidr_match][error] Weird ip (xyz) family
[snuffleupagus][xyz][cidr_match][error] Weird ip (xyz) family
[snuffleupagus][xyz][cidr_match][error] Weird ip (xyz) family
[snuffleupagus][xyz][cidr_match][error] Weird ip (xyz) family
