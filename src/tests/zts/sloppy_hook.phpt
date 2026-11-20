--TEST--
ZTS workers isolate a sloppy-comparison hook without mutating the shared handler
--EXTENSIONS--
sp_zts_test
--SKIPIF--
<?php if (!PHP_ZTS || !extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/sloppy.ini
--FILE--
<?php
$empty = var_export(dirname(__DIR__) . '/config/empty.ini', true);
$strict = 'return !in_array("1", [1]) && !in_array("yes", ["YES"]);';
var_dump(sp_zts_run(
    $strict,
    $strict,
    "sp_zts_reload($empty); return in_array('1', [1]);"
));
?>
--EXPECT--
bool(true)
