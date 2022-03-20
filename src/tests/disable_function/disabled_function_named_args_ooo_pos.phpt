--TEST--
Disable functions with out-of-order named arguments by matching argument position
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_named_args.ini
--FILE--
<?php
function foo_named_args_ooo_pos($name, $greeting='HI!', $color='red') {
    echo "boo\n";
}
foo_named_args_ooo_pos(greeting: "hello!", name: "bob");
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo_named_args_ooo_pos', because its argument 'name'%s matched a rule in %s.php on line %d