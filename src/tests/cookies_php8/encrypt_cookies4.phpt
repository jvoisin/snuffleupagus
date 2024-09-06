--TEST--
Cookie encryption in ipv6
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000 || PHP_VERSION_ID >= 80200) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies.ini
--COOKIE--
--ENV--
return <<<EOF
REMOTE_ADDR=2001:0db8:0000:0000:0000:fe00:0042:8329
HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/59.0.3071.109 Chrome/59.0.3071.109 Safari/537.36
HTTPS=1
EOF;
--FILE--
<?php
setcookie("super_cookie", "super_value");
setcookie("awful_cookie", "awful_value");
setcookie("nice_cookie", "nice_value", 1, "1", "1", true, true);
$ret = setcookie("", "Cookie with no name", 1, "1", "1", true, true);
if ($ret == TRUE) {
	echo "fail :/";
}
var_dump($_COOKIE);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: setcookie(): Argument #1 ($name) cannot be empty in %s/tests/cookies_php8/encrypt_cookies4.php:5
Stack trace:
#0 %s/tests/cookies_php8/encrypt_cookies4.php(5): setcookie('', 'Cookie with no ...', 1, '1', '1', true, true)
#1 {main}
  thrown in %s/tests/cookies_php8/encrypt_cookies4.php on line 5
