--TEST--
Cookie encryption
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies.ini
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--COOKIE--
super_cookie=1337;awful_cookie=awful_cookie_value;
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php var_dump($_COOKIE); ?>
--EXPECT--
Fatal error: [snuffleupagus][127.0.0.1][cookie_encryption][drop] Buffer underflow tentative detected in cookie encryption handling in Unknown on line 0