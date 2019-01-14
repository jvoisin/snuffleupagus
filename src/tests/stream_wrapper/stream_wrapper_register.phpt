--TEST--
Stream wrapper
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_stream_wrapper_register.ini
--FILE--
<?php 
class qwe {
  function stream_open($fname) {
    return $fname;
  }
}

stream_wrapper_register("lelel", "qwe");
stream_wrapper_register("lolol", "qwe");
fopen("lelel://asdasd", "r");
fopen("lolol://asdasd", "r");
?>
--EXPECTF--
Warning: fopen(): Unable to find the wrapper "lolol" - did you forget to enable it when you configured PHP? in %a/stream_wrapper_register.php on line %d

Warning: fopen(): file:// wrapper is disabled in the server configuration in %a/stream_wrapper_register.php on line %d

Warning: fopen(lolol://asdasd): failed to open stream: no suitable wrapper could be found in %a/stream_wrapper_register.php on line %d
