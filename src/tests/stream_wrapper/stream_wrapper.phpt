--TEST--
Stream wrapper
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) print "skip snuffleupagus extension missing";
if (!extension_loaded("openssl")) print "skip openssl extension missing";
?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_stream_wrapper.ini
--FILE--
<?php 
file_get_contents('http://qweqwezxc');
file_get_contents('https://qweqwezxc');
file_get_contents('ftp://qweqwezxc');
file_get_contents('lelel://qweqwezxc');
?>
--EXPECTF--
Warning: Unknown: Unable to find the wrapper "php" - did you forget to enable it when you configured PHP? in Unknown on line 0

Warning: Unknown: Unable to find the wrapper "php" - did you forget to enable it when you configured PHP? in Unknown on line 0

Warning: Unknown: Unable to find the wrapper "php" - did you forget to enable it when you configured PHP? in Unknown on line 0

Warning: file_get_contents(): Unable to find the wrapper "http" - did you forget to enable it when you configured PHP? in %a/stream_wrapper.php on line %d

Warning: file_get_contents(): php_network_getaddresses: getaddrinfo failed: %s

Warning: file_get_contents(https://qweqwezxc): failed to open stream: php_network_getaddresses: getaddrinfo failed: %s

Warning: file_get_contents(ftp://qweqwezxc): failed to open stream: operation failed in %a/stream_wrapper.php on line %d

Warning: file_get_contents(): Unable to find the wrapper "lelel" - did you forget to enable it when you configured PHP? in %a/stream_wrapper.php on line %d

Warning: file_get_contents(lelel://qweqwezxc): failed to open stream: No such file or directory in %a/stream_wrapper.php on line %d
