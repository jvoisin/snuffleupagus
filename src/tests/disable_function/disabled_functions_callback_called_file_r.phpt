--TEST--
Disable functions by matching on the filename_r where the callback function is called, and not defined
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_callback_called_file_r.ini
--FILE--
<?php
$dir = __DIR__;

@unlink("$dir/myfunc_callback.php");

$mycode = <<< 'EOD'
<?php

function test_callback() {
	return "Test_callback";
}

function test(callable $toto) {
	return $toto();
}
?>
EOD;

file_put_contents("$dir/myfunc_callback.php", $mycode);

include "$dir/myfunc_callback.php";

echo test('test_callback');

?>
--CLEAN--
<?php
$dir = __DIR__;
@unlink("$dir/myfunc_callback.php");
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'test_callback' in %a/myfunc_callback.php on line 4
