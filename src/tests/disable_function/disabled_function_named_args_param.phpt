--TEST--
Disable functions with named arguments by matching argument name
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_named_args.ini
--FILE--
<?php
function foo_named_args_param($name, $greeting='HI!', $color='red') {
    echo "boo\n";
}
foo_named_args_param(name: "bob", greeting: "hello!");
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo_named_args_param', because its argument '$name'%s matched a rule in %s.php on line %d