--TEST--
Disable functions - shell_exec, with a non-existing command
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions.ini
--FILE--
<?php 
$gs = exec( 'foo' );
echo "YES";
?>
--EXPECTF--
%snot found
YES
