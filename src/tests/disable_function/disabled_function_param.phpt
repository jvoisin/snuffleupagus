--TEST--
Disable functions - match on a param
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disabled_function_param.ini
--FILE--
<?php
function qweqwe($asd) {
  echo "OK\n";
}

$asd = Array(2);
qweqwe(Array(1));
qweqwe(Array(2));
?>
--EXPECTF--
OK

Fatal error: [snuffleupagus][0.0.0.0][disabled_function] Aborted execution on call of the function 'qweqwe', because its argument '$asd' content (2) matched a rule in %a/disabled_function_param.php on line 3