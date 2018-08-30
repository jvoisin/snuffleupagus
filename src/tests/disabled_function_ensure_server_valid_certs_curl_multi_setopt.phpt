--TEST--
Disable functions - Ensure that server certificates validation can't be disabled via `curl_multi_setopt`
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) { die("skip"); }
if (!extension_loaded("curl")) { die("skip"); }
?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_curl_verify_certs.ini
--FILE--
<?php
$mch = curl_multi_init();
curl_multi_setopt($mch, CURLOPT_SSL_VERIFYHOST, 0);
echo "1337";
?>
--EXPECTF--
Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'curl_multi_setopt', because its argument '$option' content (81) matched the rule 'Please don't turn CURLOPT_SSL_VERIFYHOST off.' in %s/tests/disabled_function_ensure_server_valid_certs_curl_multi_setopt.php on line %d
