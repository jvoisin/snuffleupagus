--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
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
echo strlen($test->$arg) . "\n";
$test = new test_class('qwe', 'nop_object');
echo strlen($test->$arg) . "\n";
?>
--EXPECTF--
3
[snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strlen' in %a/tests/disabled_function_local_var_obj.php:%d.
