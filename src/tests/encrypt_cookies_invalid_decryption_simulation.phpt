--TEST--
Cookie encryption - invalid decryption in simulation mode
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies_simulation.ini
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--COOKIE--
super_cookie=Wk9NR1RISVNJU05PVEVOQ1JZUFRFREFUQUxMV0hBVFRIRUhFTExJU0hIRUxMQVJFWU9VRE9JTkdaT01Hb2htYXliZXRoaXNpc2Fub2xkc2Vzc2lvbmNvb2tpZQo=;awfulcookie=awfulcookievalue;
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php
echo "1337\n";
var_dump($_COOKIE); ?>
--EXPECT--
Warning: [snuffleupagus][cookie_encryption] Something went wrong with the decryption of super_cookie. Using the cookie 'as it' instead of decrypting it in Unknown on line 0
1337
array(2) {
  ["super_cookie"]=>
  string(124) "Wk9NR1RISVNJU05PVEVOQ1JZUFRFREFUQUxMV0hBVFRIRUhFTExJU0hIRUxMQVJFWU9VRE9JTkdaT01Hb2htYXliZXRoaXNpc2Fub2xkc2Vzc2lvbmNvb2tpZQo="
  ["awfulcookie"]=>
  string(16) "awfulcookievalue"
}