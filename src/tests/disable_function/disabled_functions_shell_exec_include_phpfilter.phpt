--TEST--
Disable functions - shell_exec via include(php://filter)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 70400) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_extra.ini
allow_url_include=1
--FILE--
<?php
include('php://filter//resource=data://text/plain,'.urlencode('<?php shell_exec("ls");'));
?>
--EXPECTF--
Deprecated: Directive 'allow_url_include' is deprecated in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'shell_exec', %a matched a rule in php%a line 1
