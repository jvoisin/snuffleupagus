--TEST--
Cookie encryption - no encryption key specified
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/encrypt_cookies_no_key.ini
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
<?php echo "1\n\n\n\n\n"; ?>
--EXPECT--
1
