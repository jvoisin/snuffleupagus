--TEST--
Cookie encryption - empty environment variable specified
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_encrypted_cookies_empty_env.ini
display_errors=1
display_startup_errors=1
error_reporting=E_ALL
--COOKIE--
super_cookie=cGFkZGluZ3BhZGRpbmdwYWRkaW5ncGFkZGluZ3BhZGRpbmdwYWRkaW5ncGFkZGluZ3BhZGRpbmdwYWRkaW5ncGFkZGluZwo=
--FILE--
<?php echo "1\n\n\n\n\n"; ?>
--EXPECT--
Warning: [snuffleupagus][cookie_encryption] The environment variable 'SUPER_ENV_VAR' is empty, cookies are weakly encrypted in Unknown on line 0

Warning: [snuffleupagus][cookie_encryption] Something went wrong with the decryption of super_cookie in Unknown on line 0
1
