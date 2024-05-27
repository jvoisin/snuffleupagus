--TEST--
Stream wrapper (php)
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) print "skip snuffleupagus extension missing";
?>
--INI--
sp.configuration_file={PWD}/config/config_stream_wrapper_php.ini
--FILE--
<?php
echo file_get_contents('php://input');
file_put_contents('php://output', "Hello from stdout\n");
file_put_contents('php://stderr', "Hello from stderr #1\n");

file_put_contents('php://memory', "Bye from memory\n");
echo file_get_contents('php://memory');

file_put_contents('php://temp', "Bye from temp\n");
echo file_get_contents('php://temp');

file_put_contents('php://stderr', "Hello from stderr #2\n");

file_put_contents('php://filter/write=string.toupper/resource=output.tmp', "Hello from stdout filtered\n");
echo file_get_contents('php://filter/read=string.toupper/resource=output.tmp');

$foo = stream_wrapper_unregister("php");
fwrite(STDERR, $foo);
file_put_contents('php://stderr', "Hello from stderr #3\n");

stream_wrapper_restore("php");
file_put_contents('php://stderr', "Hello from stderr #4\n");
file_put_contents('php://memory', "Bye from memory\n");
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "input" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_get_contents(php://input): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d

Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "output" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_put_contents(php://output): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d
Hello from stderr #1

Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "memory" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_put_contents(php://memory): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d

Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "memory" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_get_contents(php://memory): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d

Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "temp" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_put_contents(php://temp): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d

Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "temp" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_get_contents(php://temp): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d
Hello from stderr #2

Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "filter" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_put_contents(php://filter/write=string.toupper/resource=output.tmp): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d

Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "filter" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_get_contents(php://filter/read=string.toupper/resource=output.tmp): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d
1
Warning: file_put_contents(): Unable to find the wrapper "php" - did you forget to enable it when you configured PHP? in %a/stream_wrapper_php.php on line %d

Warning: file_put_contents(): file:// wrapper is disabled in the server configuration in %a/stream_wrapper_php.php on line %d

Warning: file_put_contents(php://stderr): %s to open stream: no suitable wrapper could be found in %a/stream_wrapper_php.php on line %d
Hello from stderr #4

Warning: [snuffleupagus][0.0.0.0][wrappers_whitelist][log] Call to not allowed php stream type "memory" dropped in %a/stream_wrapper_php.php on line %d

Warning: file_put_contents(php://memory): %s to open stream: operation failed in %a/stream_wrapper_php.php on line %d
