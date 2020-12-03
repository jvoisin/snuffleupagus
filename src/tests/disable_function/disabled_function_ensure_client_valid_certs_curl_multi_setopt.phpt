--TEST--
Disable functions - Ensure that client certificates validation can't be disabled via `curl_multi_setopt`
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) { print("skip"); }
if (!extension_loaded("curl")) { print("skip"); }
?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--EXTENSIONS--
curl
--INI--
sp.configuration_file={PWD}/config/disabled_function_curl_verify_certs.ini
--FILE--
<?php
$mch = curl_multi_init();
curl_multi_setopt($mch, CURLOPT_SSL_VERIFYPEER, 0);
echo "1337";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'curl_multi_setopt', because its argument '$option' content (64) matched the rule 'Please don't turn CURLOPT_SSL_VERIFYPEER off.' in %s/disabled_function_ensure_client_valid_certs_curl_multi_setopt.php on line %d
