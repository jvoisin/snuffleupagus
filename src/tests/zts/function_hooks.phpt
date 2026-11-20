--TEST--
ZTS workers retain stacked function hooks when another worker reloads configuration
--EXTENSIONS--
sp_zts_test
--SKIPIF--
<?php if (!PHP_ZTS || !extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/functions.ini
--FILE--
<?php
$empty = var_export(dirname(__DIR__) . '/config/empty.ini', true);
$protected = <<<'PHP'
set_error_handler(static function ($level, $message) {
    if (strpos($message, '[disabled_function]') !== false) {
        throw new RuntimeException('protected');
    }
    return false;
});
rand();
try {
    str_rot13('probe');
} catch (RuntimeException $e) {
    return $e->getMessage() === 'protected';
}
return false;
PHP;
var_dump(sp_zts_run(
    $protected,
    $protected,
    "sp_zts_reload($empty); return str_rot13('probe') === 'cebor';"
));
?>
--EXPECT--
bool(true)
