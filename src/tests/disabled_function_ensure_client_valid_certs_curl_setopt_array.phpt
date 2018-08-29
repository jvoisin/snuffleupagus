--TEST--
Disable functions - Ensure that client certificates validation can't be disabled via `curl_setopt_array`
--EXTENSIONS--
curl
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
$options = array(CURLOPT_SSL_VERIFYHOST => 0);
curl_setopt_array($ch, $options);
echo "1337";
?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'curl_setopt_array' in Command line code:1, because its argument '$options' content (0) matched the rule 'Please don't turn CURLOPT_SSL_VERIFYHOST off.'.
