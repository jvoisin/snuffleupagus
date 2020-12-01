--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var_2.ini
--FILE--
<?php 
namespace qwe {
  const UNE_CONSTANTE = 'constant';
}
namespace asd {
$b = Array();
$b['_GET[obj->nop]'] = Array();
$b['_GET[obj->nop]']["qwe"] = Array();
$b[456] = Array();
$b[456]['zxc'] = "qwe";
$b[456]['nop'] = "nop";
$b['_GET[obj->nop]']["qwe"][321] = "Yeay";
$b["123"] = "qwe";
$b["123a"] = "foo";
$b["asd"] = "zxc";
$b['_GET]["obj->nop::qwe']["qwe"][1337] = (Object)(['uio' => "valeur de apres"]);
$b['_GET]["obj->nop::qwe']["qwe"][1338] = (Object)(['uio' => "valeur de a"]);
$c = (Object)(['qwe' => Array(\qwe\UNE_CONSTANTE => 'zxc')]);
$idk = 'test_asd';
$class_name = 'test_object';
class test_object {
  const TEST_VALUE = ['constant' => 'truc'];
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
$d = new test_object($c);
$a = 1338;
function test(){
  strtoupper("qwe");
}
echo "Valeur: " . $b['_GET]["obj->nop::qwe']["qwe"][$a]->uio . "\n";
test();

$a = 1337;
echo "Valeur: " . $b['_GET]["obj->nop::qwe']["qwe"][$a]->uio . "\n";
test();
}
?>
--EXPECTF--
Valeur: valeur de a
Valeur: valeur de apres

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_local_var_4.php on line 36