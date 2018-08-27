--TEST--
Cookie encryption - empty environment variable specified
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_regexp_cookies_empty_env.ini
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--COOKIE--
super_cookie=1337;awful_cookie=awful_cookie_value;
--ENV--
return <<<EOF
NOT_REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php echo "1\n\n\n\n\n"; ?>
--EXPECT--
Fatal error: [snuffleupagus][cookie_encryption] Buffer underflow tentative detected in cookie encryption handling in Unknown on line 0