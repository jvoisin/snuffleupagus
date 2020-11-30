--TEST--
Cookie encryption
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_regexp_cookies.ini
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--COOKIE--
super_cookie=jWjORGsgZyqzk3WA63XZBmUoSknXWnXDfAAAAAAAAAAAAAAAAAAAAAA7LiMDfkpP94jDnMVH%2Fm41GeL0Y00q3mbOFYz%2FS9mQGySu;awful_cookie=awful_cookie_value;
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php var_dump($_COOKIE); ?>
--EXPECT--
Warning: [snuffleupagus][127.0.0.1][cookie_encryption][log] Something went wrong with the decryption of super_cookie in Unknown on line 0
array(1) {
  ["awful_cookie"]=>
  string(18) "awful_cookie_value"
}
