--TEST--
Disable functions by matching on the filename where the function is called, and not defined
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_called_file.ini
--FILE--
<?php
$dir = __DIR__;

@unlink("$dir/myfunc.php");

$code = <<< EOD
<?php
function test() {
	echo "TOTO";
}
?>
EOD;

file_put_contents("$dir/myfunc.php", $code);

include "$dir/myfunc.php";

test();

?>
--EXPECTF--
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'test' in %a/disabled_functions_called_file.php:18 has been disabled.
--CLEAN--
<?php
$dir = __DIR__;
@unlink("$dir/myfunc.php");
?>