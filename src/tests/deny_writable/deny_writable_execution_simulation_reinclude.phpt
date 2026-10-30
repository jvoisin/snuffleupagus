--TEST--
Readonly execution attempt (simulation mode): a file modified and included again during the request is checked again
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) { print "skip"; };

// root has write privileges on any file
if (TRUE == function_exists("posix_getuid")) {
	if (0 == posix_getuid()) {
		print "skip Running as root";
	}
} elseif (TRUE == function_exists("shell_exec")) {
	if ("root" == trim(shell_exec("whoami"))) {
		print "skip Running as root";
	}
}
 ?>
--INI--
sp.configuration_file={PWD}/config/config_disable_writable_simulation.ini
--FILE--
<?php 
$file = __DIR__ . "/reinclude.txt";

@chmod($file, 0777);
@unlink($file);

file_put_contents($file, '<?php function from_first_version() {} echo "first version\n";');
chmod($file, 0400);
include $file;
from_first_version();

chmod($file, 0777);
file_put_contents($file, '<?php echo "second version\n";');
touch($file, time() + 10);
clearstatcache();
include $file;
from_first_version();

// not in --CLEAN--: it would run from a writable file too, and its warning would bork the test
unlink($file);
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/deny_writable_execution_simulation_reinclude.php) in %a/deny_writable_execution_simulation_reinclude.php on line %d

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a file owned by the PHP process (%a/reinclude.txt) in %a/deny_writable_execution_simulation_reinclude.php on line 9

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a file owned by the PHP process (%a/reinclude.txt) in %a/reinclude.txt on line 1
first version

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/reinclude.txt) in %a/deny_writable_execution_simulation_reinclude.php on line 16

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/reinclude.txt) in %a/reinclude.txt on line 1
second version
