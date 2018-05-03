--TEST--
Disable functions by matching on the filename_r where the callback function is called, and not defined
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
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
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'test_callback' in %a/disabled_functions_callback__called_file_r.php:%d.
--XFAIL--
--CLEAN--
<?php
$dir = __DIR__;
@unlink("$dir/myfunc_callback.php");
?>