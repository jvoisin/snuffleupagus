--TEST--
Disable functions by matching the calltrace, ending on a method
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_chain_method.ini
--FILE--
<?php

class Foo {
    function bar() { return 42; }
}

function caller($o) {
    return $o->bar();
}

$o = new Foo();
echo "direct: " . $o->bar() . "\n";
echo "via caller:\n";
caller($o);
echo "unreached\n";
?>
--EXPECTF--
direct: 42
via caller:

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'caller>Foo::bar' in %a/disabled_functions_chain_method.php on line 4
