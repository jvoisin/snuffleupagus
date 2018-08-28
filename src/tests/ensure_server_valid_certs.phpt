--TEST--
Disable functions - Ensure that server certificates validation can't be disabled
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) die "skip"; 
if (!extension_loaded("curl")) die "skip"; 
?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_curl_verify_certs.ini
--FILE--
<?php
$ch = curl_init();
curl_setopt($ch, CURLOPT_VERBOSE, '1'); 
curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, '0'); 
echo "1337";
?>
--EXPECTF--
[snuffleupagus][verify_vertificates] Please don't deactivate client certificate validation in %s/tests/ensure_server_valid_certs.php on line %d
