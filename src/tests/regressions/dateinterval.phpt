--TEST--
Test for DateInterval
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print("skip"); ?>
--INI--
sp.configuration_file={PWD}/../config/DateInterval.ini
--FILE--
<?php
$a = new DateInterval('PT0S');
echo "ok";
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'DateInterval::__construct' in %s/tests/regressions/dateinterval.php on line 2
ok

