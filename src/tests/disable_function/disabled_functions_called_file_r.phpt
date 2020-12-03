--TEST--
Disable functions by matching on the filename_r where the function is called, and not defined
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_called_file_r.ini
--FILE--
<?php
$dir = __DIR__;

@unlink("$dir/myfunc.php");

$mycode = <<< EOD
<?php
function test() {
	echo "TOTO";
}
?>
EOD;

file_put_contents("$dir/myfunc.php", $mycode);

include "$dir/myfunc.php";

test();

?>
--CLEAN--
<?php
$dir = __DIR__;
@unlink("$dir/myfunc.php");
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'test' in %a/myfunc.php on line 3
