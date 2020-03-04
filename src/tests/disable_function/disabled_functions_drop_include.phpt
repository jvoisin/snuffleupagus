--TEST--
Disable function, bug : https://github.com/jvoisin/snuffleupagus/issues/181
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_functions_drop_include.ini
--FILE--
<?php
$dir = __DIR__;

@unlink("$dir/test_include.php");

$code = <<< 'EOD'
<?php
$test = "testOK";
?>
EOD;

file_put_contents("$dir/test_include.php", $code);

include "$dir/test_include.php";

echo $test;

?>

--EXPECTF--
testOK
