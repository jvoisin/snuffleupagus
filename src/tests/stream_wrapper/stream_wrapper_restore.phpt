--TEST--
Stream wrapper
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_stream_wrapper_register.ini
--FILE--
<?php 
stream_wrapper_restore("file");
fopen("file://asdasd", "r");
?>
--EXPECTF--
Notice: stream_wrapper_restore(): file:// was never changed, nothing to restore in %a/stream_wrapper_restore.php on line %d

Warning: fopen(): Unable to find the wrapper "file" - did you forget to enable it when you configured PHP? in %a/stream_wrapper_restore.php on line %d

Warning: fopen(file://asdasd): failed to open stream: No such file or directory in %a/stream_wrapper_restore.php on line %d
