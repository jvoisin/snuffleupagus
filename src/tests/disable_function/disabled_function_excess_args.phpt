--TEST--
Disable functions with excess arguments
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_excess_args.ini
--FILE--
<?php
function foo_excess_args($name, $greeting='HI!', $color='red') {
    echo "boo\n";
}
foo_excess_args("bob", "hi", "green", "blubb");
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo_excess_args' in %s.php on line %d