--TEST--
input filter: server_strip
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/filter.ini
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--ENV--
return <<<EOF
HTTP_USER_AGENT=Mozilla/5.0 (Windows NT 6.0; rv:29.0) <script>alert('123');</script>Gecko/20100101 Firefox/29.0
EOF;
--COOKIE--
--GET--
--POST--
--FILE--
<?php
var_dump($_SERVER['HTTP_USER_AGENT']);
--EXPECT--
string(95) "Mozilla/5.0 (Windows NT 6.0; rv:29.0) _script_alert(_123_);_/script_Gecko/20100101 Firefox/29.0"
