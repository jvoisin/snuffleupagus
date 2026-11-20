--TEST--
ZTS workers preserve original INI callbacks and isolate protection on reload
--EXTENSIONS--
sp_zts_test
--SKIPIF--
<?php if (!PHP_ZTS || !extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/ini.ini
--FILE--
<?php
$empty = var_export(dirname(__DIR__) . '/config/empty.ini', true);
var_dump(sp_zts_run(
    'return ini_set("precision", "12") !== false && ini_get("precision") === "12"
        && ini_set("highlight.comment", "#000aBc") !== false
        && ini_get("sendmail_from") === "thread@example.com";',
    'set_error_handler(static function () { return true; });
     return ini_set("precision", "13") !== false && ini_get("precision") === "13"
        && ini_set("precision", "1") === false;',
    "sp_zts_reload($empty);
     return ini_set('precision', '1') !== false && ini_get('precision') === '1';"
));
?>
--EXPECT--
bool(true)
