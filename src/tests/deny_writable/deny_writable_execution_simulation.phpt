--TEST--
Readonly execution attempt (simulation mode)
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) print "skip";

// root has write privileges on any file
if (TRUE == function_exists("posix_getuid")) {
	if (0 == posix_getuid()) {
		print "skip";
	}
} elseif (TRUE == function_exists("shell_exec")) {
	if ("root" == trim(shell_exec("whoami"))) {
		print "skip";
	}
}
 ?>
--INI--
sp.configuration_file={PWD}/config/config_disable_writable_simulation.ini
--FILE--
<?php 
$dir = __DIR__;

// just in case
@unlink("$dir/non_writable_file.txt");
@unlink("$dir/writable_file.txt");

file_put_contents("$dir/writable_file.txt", '<?php echo "Code execution within a writable file.\n";');
file_put_contents("$dir/non_writable_file.txt", '<?php echo "Code execution within a non-writable file.\n";');
chmod("$dir/writable_file.txt", 0777);
chmod("$dir/non_writable_file.txt", 0400);
include "$dir/writable_file.txt";
include "$dir/non_writable_file.txt";
?>
--CLEAN--
<?php
$dir = __DIR__;
chmod("$dir/non_writable_file.txt", 0777);
chmod("$dir/writable_file.txt", 0777);
unlink("$dir/non_writable_file.txt");
unlink("$dir/writable_file.txt");
?>
--EXPECTF--
Warning: [snuffleupagus][readonly_exec] Attempted execution of a writable file (%a/deny_writable_execution_simulation.php). in %a/deny_writable_execution_simulation.php on line 2

Warning: [snuffleupagus][readonly_exec] Attempted execution of a writable file (%a/writable_file.txt). in %a/deny_writable_execution_simulation.php on line 12

Warning: [snuffleupagus][readonly_exec] Attempted execution of a writable file (%a/writable_file.txt). in %a/writable_file.txt on line 1
Code execution within a writable file.
Code execution within a non-writable file.
