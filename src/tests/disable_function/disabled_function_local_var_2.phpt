--TEST--
Disable functions - match on a local variable
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_local_var.ini
--FILE--
<?php 
$a = 1338;
function test(){
    echo strtoupper("id") . "\n";
}
echo "Value of a: $a\n";
test();

$a = Array();
$a['qwe'] = 'block';
$a['123'] = 'nop';
echo "Value of a:\n";
var_dump($a);
test();

$a['123'] = 'block';
echo "Value of a:\n";
var_dump($a);
test();
?>
--EXPECTF--
Value of a: 1338
ID
Value of a:
array(2) {
  ["qwe"]=>
  string(5) "block"
  [123]=>
  string(3) "nop"
}
ID
Value of a:
array(2) {
  ["qwe"]=>
  string(5) "block"
  [123]=>
  string(5) "block"
}

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'strtoupper' in %a/disabled_function_local_var_2.php on line 4
