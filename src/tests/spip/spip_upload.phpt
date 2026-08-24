--TEST--
SPIP rules - upload hardening: move_uploaded_file() to a .php destination is blocked
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../../config/spip.rules
--FILE--
<?php
move_uploaded_file("/tmp/source", "/var/www/html/shell.php");
echo "NOT-BLOCKED\n";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'move_uploaded_file'%a
