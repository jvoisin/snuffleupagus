--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
namespace asd {
function test(){
  strlen("qwe");
}
echo "Valeur: " . \asd\test_object::TEST_VALUE . "\n";
test();
class test_object {
  const TEST_VALUE = "qwerty";
  private $asd = "qwe";
  public $qwe = 'bar';
  private $test_asd = '';
  function __construct($asd) {
    $this->test_asd = $asd;
  }
  function do_a_barell_roll() {
    var_dump($this->test_asd);
  }
}
}
?>
--EXPECTF--
Valeur: qwerty
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/tests/disabled_function_local_var_7.php:%d has been disabled.
