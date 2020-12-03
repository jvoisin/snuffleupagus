--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip" ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var_obj.ini
--FILE--
<?php
class test_class {
  public $qwe = 'value';
  function __construct($arg, $value) {
    $this->$arg = $value;
  }
}

$test_array = ['qwe'];
$arg = 'qwe';
$test = new test_class('qwe', 'qwe');
echo strtoupper($test->$arg) . "\n";
$test = new test_class('qwe', 'nop_object');
echo strtoupper($test->$arg) . "\n";
?>
--EXPECTF--
QWE

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_local_var_obj.php on line 14
