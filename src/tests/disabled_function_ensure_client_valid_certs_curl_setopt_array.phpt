--TEST--
Disable functions - Ensure that client certificates validation can't be disabled via `curl_setopt_array`
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) { die("skip"); }
if (!extension_loaded("curl")) { die("skip"); }
?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_curl_verify_certs.ini
--FILE--
<?php
$ch = curl_init();
curl_setopt($ch, CURLOPT_VERBOSE, '1');
$options = array(CURLOPT_SSL_VERIFYPEER => 0);
curl_setopt_array($ch, $options);
echo "1337";
?>
--EXPECTF--
Fatal error: [snuffleupagus][disabled_function] Aborted execution on call of the function 'curl_setopt_array', because its argument '$options' content (0) matched the rule 'Please don't turn CURLOPT_SSL_VERIFYPEER off.' in %s/tests/disabled_function_ensure_client_valid_certs_curl_setopt_array.php on line 5
