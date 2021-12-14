--TEST--
Disable functions - shell_exec via signal handler
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 70100) print "skip"; ?>
--EXTENSIONS--
pcntl
--INI--
sp.configuration_file={PWD}/config/disabled_functions_extra.ini
pcntl.async_signals=1
--FILE--
<?php
declare(ticks=1);
ini_set("pcntl.async_signals", "1");
pcntl_signal(SIGALRM, function($signo) { shell_exec("ls"); });
system("kill -14 " . getmypid());
sleep(5);
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'shell_exec', %a matched a rule in %a.php on line 4
