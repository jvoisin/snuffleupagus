--TEST--
Cookie encryption in ipv4
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies.ini
--COOKIE--
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/59.0.3071.109 Chrome/59.0.3071.109 Safari/537.36
HTTPS=1
EOF;
--FILE--
<?php
setcookie("super_cookie", "super_value");
setcookie("awful_cookie", "awful_value");
setcookie("nice_cookie", "nice_value", 1, "1", "1", true, true);
var_dump($_COOKIE);
?>
--EXPECT--
array(0) {
}
