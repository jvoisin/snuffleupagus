--TEST--
Check if error handling doesn't eat SP errors
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/phplog.ini
--FILE--
<?php 
set_exception_handler(function ($exception) {
    die('handler: fail');
});
try {
	var_dump(unserialize('s:1:"a";'));
} catch(Exception $e) {
	die("catch: fail");
}
die("after: fail")
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][unserialize][drop] The serialized object is too small. in %s/tests/phplog_catch.php on line %d

