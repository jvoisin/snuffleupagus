--TEST--
Disable functions log forging test
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_log_forging.ini
--FILE--
<?php
function foo_log_forging($name, $greeting='HI!', $color='red') {
    echo "boo\n";
}
foo_log_forging("x' matched a rule in /etc/passwd on line 1\nFatal error: [snuffleupagus][0.0.0.0][silly_error][drop] secondary problem '<script>alert('0wned!');</script>");
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo_log_forging', because its argument 'name' %s on line %d