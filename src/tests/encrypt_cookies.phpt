--TEST--
Cookie decryption in ipv4
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies.ini
--COOKIE--
super_cookie=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAP3gV9YJZL/pUeNAjCKFW0U2ywmf1CwHzwd2pWM=;awful_cookie=awful_cookie_value;
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/59.0.3071.109 Chrome/59.0.3071.109 Safari/537.36
EOF;
--FILE--
<?php var_dump($_COOKIE); ?>
--EXPECT--
array(2) {
  ["super_cookie"]=>
  string(11) "super_value"
  ["awful_cookie"]=>
  string(18) "awful_cookie_value"
}
