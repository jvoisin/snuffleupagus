--TEST--
Cookie decryption in ipv4
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_regexp_cookies_bad_regexp.ini
error_reporting=1
--COOKIE--
super_cookie=IpRZV4rivSjANrEOSxINd%2FdFe17giJgaAAAAAAAAAAAAAAAAAAAAALnmBVs%2BTILKxauHeGcUyJpR%2BX2UiZ6OamUTaWc=;awful_cookie=awful_cookie_value;
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/59.0.3071.109 Chrome/59.0.3071.109 Safari/537.36
EOF;
--FILE--
<?php var_dump($_COOKIE); ?>
--EXPECTF--
%AFatal error: [snuffleupagus][127.0.0.1][config][log] Invalid regexp '^super_co[a-z+$' for '.name_r()' on line 2 in Unknown on line 0
