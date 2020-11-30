--TEST--
Cookie decryption with ipv6
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_regexp_cookies.ini
--COOKIE--
super_cookie=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABM84SCotZTpP6b27Lr5lavORPMvqaKpcUahvxw=;awful_cookie=awful_cookie_value;
--ENV--
return <<<EOF
REMOTE_ADDR=2001:0db8:0000:0000:0000:fe00:0042:8329
HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/59.0.3071.109 Chrome/59.0.3071.109 Safari/537.36
HTTPS=1
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
