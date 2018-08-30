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
<?php echo "1"; ?>
--EXPECT--
Fatal error: [snuffleupagus][config] Invalid configuration file in Unknown on line 0

Fatal error: [snuffleupagus][config] You're trying to use the cookie encryption featureon line 2 without having set the `.encryption_key` option in`sp.global`: please set it first in Unknown on line 0
