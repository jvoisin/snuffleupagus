--TEST--
Disable functions - chmod
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip Not implemented in PHP 8+"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_chmod.ini
--FILE--
<?php
chmod( 'foo', 0777 );
?>
--XFAIL--
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'chmod', because its argument '$mode' content (511) matched a rule in %a.php on line %d
