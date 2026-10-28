--TEST--
Readonly execution attempt (simulation mode): each file is checked once per request, not on every function call
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

@chmod("$dir/writable_functions.txt", 0777);
@unlink("$dir/writable_functions.txt");

file_put_contents("$dir/writable_functions.txt", '<?php function from_writable_file() { return "called\n"; }');
chmod("$dir/writable_functions.txt", 0777);
include "$dir/writable_functions.txt";
echo from_writable_file();
echo from_writable_file();
echo from_writable_file();
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/writable_functions.txt");
?>
--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/deny_writable_execution_simulation_once.php) in %a/deny_writable_execution_simulation_once.php on line %d

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/writable_functions.txt) in %a/deny_writable_execution_simulation_once.php on line %d

Warning: [snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/writable_functions.txt) in %a/writable_functions.txt on line %d
called
called
called
