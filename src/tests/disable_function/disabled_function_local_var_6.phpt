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
  strtoupper("qwe");
}
echo "Valeur: " . \asd\test_object::TEST_VALUE['constant'] . "\n";
test();
class test_object {
  const TEST_VALUE = ['constant' => 'no good'];
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
Valeur: no good

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_local_var_6.php on line 4