--TEST--
Strict mode disabled
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) print "skip snuffleupagus extension missing";
?>
--INI--
sp.configuration_file={PWD}/config/config_strict_mode_disabled.ini
--FILE--
<?php 
ini_set('display_errors', 1);
?>
--EXPECTF--
