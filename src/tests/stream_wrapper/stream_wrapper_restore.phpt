--TEST--
Stream wrapper
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_stream_wrapper_register.ini
--FILE--
<?php 
stream_wrapper_restore("stdin");
fopen("stdin://asdasd", "r");
?>
--EXPECTF--
%s

Warning: fopen(): Unable to find the wrapper "stdin" - did you forget to enable it when you configured PHP? in %a/stream_wrapper_restore.php on line %d

Warning: fopen(stdin://asdasd): failed to open stream: No such file or directory in %a/stream_wrapper_restore.php on line %d
