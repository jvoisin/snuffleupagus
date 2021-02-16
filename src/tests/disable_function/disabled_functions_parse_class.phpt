--TEST--
Disable functions - Parsing of an Object as a return value of a function
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_ret.ini
--FILE--
<?php 
/*
Because Snuffleupagus used to cast everything with the `zval_get_string` function,
this sometimes raised exceptions, because PHP is awful.
 */
class Bob {
    function a() {
        return new StdClass;
    }
}
$b = new Bob;
echo ($b->a() instanceof StdClass)?'Y':'N';
?>
--EXPECT--
Y
