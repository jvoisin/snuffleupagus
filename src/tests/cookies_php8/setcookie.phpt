--TEST--
Set cookies.
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
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
setcookie("name");
setcookie("super_cookie");
setcookie("name", "value");
setcookie("name", "value1", 1);
setcookie("name", "value2", 0);
setcookie("name", "value", 1, "/super/path");
setcookie("name", "value", 1, "/super/path", "super_domain");
setcookie("name", "value", 1, "/super/path", "super_domain1", true);
setcookie("name", "value", 1, "/super/path", "super_domain2", false);
setcookie("name", "value", 1, "/super/path", "super_domain1", true, true);
setcookie("name", "value", 1, "/super/path", "super_domain2", true, false);
setcookie("name", "value", 1, "/super/path", "super_domain2", true, false, 1337);
setcookie();
echo '1337';
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: setcookie() expects at most 7 arguments, 8 given in %s/tests/cookies_php8/setcookie.php:13
Stack trace:
#0 %s/tests/cookies_php8/setcookie.php(13): setcookie('name', 'value', 1, '/super/path', 'super_domain2', true, false, 1337)
#1 {main}
  thrown in %s/tests/cookies_php8/setcookie.php on line 13
