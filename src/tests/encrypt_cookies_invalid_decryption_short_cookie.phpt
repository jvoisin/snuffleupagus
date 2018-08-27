--TEST--
Cookie encryption - invalid decryption in simulation mode with a short cookie
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies_simulation.ini
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--COOKIE--
super_cookie=AAA;awful_cookie=awful_cookie_value;
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php var_dump($_COOKIE); ?>
--EXPECT--
Warning: [snuffleupagus][cookie_encryption] Buffer underflow tentative detected in cookie encryption handling for super_cookie. Using the cookie 'as it' instead of decrypting it in Unknown on line 0
array(2) {
  ["super_cookie"]=>
  string(3) "AAA"
  ["awful_cookie"]=>
  string(18) "awful_cookie_value"
}