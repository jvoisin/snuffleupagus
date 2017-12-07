--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
function test(){
    echo strlen("id") . "\n";
}

$a = Array();
$c = '123';
$a['qwe'] = 'qwe';
$a['123'] = 'nop';
echo "Value of a:\n";
var_dump($a);
test();

$a[$c] = (Object)['prop' => 'block'];
echo "Value of a:\n";
var_dump($a);
test();
?>
--EXPECTF--
Value of a:
array(2) {
  ["qwe"]=>
  string(3) "qwe"
  [123]=>
  string(3) "nop"
}
2
Value of a:
array(2) {
  ["qwe"]=>
  string(3) "qwe"
  [123]=>
  object(stdClass)#1 (1) {
    ["prop"]=>
    string(5) "block"
  }
}
[snuffleupagus][0.0.0.0][disabled_function][drop] The call to the function 'strlen' in %a/tests/disabled_function_local_var_3.php:%d has been disabled.
