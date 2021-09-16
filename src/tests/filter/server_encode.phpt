--TEST--
input filter: server_encode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/filter.ini
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--ENV--
return <<<EOF
REQUEST_URI=AAA<>"'`!AAA
EOF;
--COOKIE--
--GET--
BBB<>"'`!BBB
--POST--
--FILE--
<?php
var_dump($_SERVER['REQUEST_URI']);
var_dump($_SERVER['QUERY_STRING']);
--EXPECT--
string(22) "AAA%3C%3E%22%27%60!AAA"
string(22) "BBB%3C%3E%22%27%60!BBB"

