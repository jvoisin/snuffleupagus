--TEST--
Broken configuration - unmatching brackets
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_unmatching_brackets.ini
--FILE--
--EXPECTF--
[snuffleupagus][0.0.0.0][config][error] The string 'arr[b]]]]]' contains unbalanced brackets.
