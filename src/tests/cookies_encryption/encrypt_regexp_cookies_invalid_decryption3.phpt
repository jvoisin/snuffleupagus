--TEST--
Cookie encryption
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_regexp_cookies.ini
--COOKIE--
super_cookie=;awful_cookie=awful_cookie_value;
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php var_dump($_COOKIE); ?>
--EXPECT--
array(2) {
  ["super_cookie"]=>
  string(0) ""
  ["awful_cookie"]=>
  string(18) "awful_cookie_value"
}
