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
1
