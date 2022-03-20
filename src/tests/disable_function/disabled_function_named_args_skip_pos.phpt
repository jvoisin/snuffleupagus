--TEST--
Disable functions with named arguments (skipping opt. args) by matching argument position
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_named_args.ini
--FILE--
<?php
function foo_named_args_skip_pos($name, $greeting='HI!', $color='red') {
    echo "boo\n";
}
foo_named_args_skip_pos("bob", color: "green");
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'foo_named_args_skip_pos', because its argument 'color'%s matched a rule in %s.php on line %d