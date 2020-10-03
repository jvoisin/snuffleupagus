--TEST--
Stream wrapper
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 70400) { die("skip BROKEN with 7.4"); } ?>
--INI--
sp.configuration_file={PWD}/config/config_stream_wrapper_register.ini
--FILE--
<?php 
stream_wrapper_restore("stdin");
fopen("stdin://asdasd", "r");
?>
--EXPECTF--
Notice: stream_wrapper_restore(): stdin:// was never changed, nothing to restore in %a/stream_wrapper_restore.php on line %d

Warning: fopen(): Unable to find the wrapper "stdin" - did you forget to enable it when you configured PHP? in %a/stream_wrapper_restore.php on line %d

Warning: fopen(stdin://asdasd): failed to open stream: No such file or directory in %a/stream_wrapper_restore.php on line %d
