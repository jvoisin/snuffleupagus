--TEST--
Readonly execution attempt (simulation mode)
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
$dir = __DIR__;

// just in case
@chmod("$dir/non_writable_file.txt", 0777);
@chmod("$dir/writable_file.txt", 0777);
@unlink("$dir/non_writable_file.txt");
@unlink("$dir/writable_file.txt");

file_put_contents("$dir/writable_file.txt", '<?php echo "Code execution within a writable file.\n";');
file_put_contents("$dir/non_writable_file.txt", '<?php echo "Code execution within a non-writable file.\n";');
chmod("$dir/writable_file.txt", 0777);
chmod("$dir/non_writable_file.txt", 0400);
include "$dir/writable_file.txt";
include "$dir/non_writable_file.txt";
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/deny_writable_execution_simulation.php) in %a/deny_writable_execution_simulation.php on line %d

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/writable_file.txt) in %a/deny_writable_execution_simulation.php on line %d

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/writable_file.txt) in %a/writable_file.txt on line %d
Code execution within a writable file.

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a file owned by the PHP process (%s/tests/deny_writable/non_writable_file.txt) in %s/tests/deny_writable/deny_writable_execution_simulation.php on line %d

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a file owned by the PHP process (%s/tests/deny_writable/non_writable_file.txt) in %src/tests/deny_writable/non_writable_file.txt on line %d
Code execution within a non-writable file.
