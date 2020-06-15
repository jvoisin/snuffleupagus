--TEST--
Readonly execution attempt
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) print "skip";

$filename = __DIR__ . '/test.txt';

@unlink($filename);

file_put_contents($filename, 'a');
chmod($filename, 0400);

if (is_writable($filename)) print "skip";
@unlink($filename);
 ?>
--INI--
sp.configuration_file={PWD}/config/config_disable_writable.ini
--FILE--
<?php 
$dir = __DIR__;

// just in case
@unlink("$dir/non_writable_file.txt");
@unlink("$dir/writable_file.txt");

file_put_contents("$dir/non_writable_file.txt", '<?php echo "Code execution within a non-writable file.\n";');
file_put_contents("$dir/writable_file.txt", '<?php echo "Code execution within a writable file.\n";');
chmod("$dir/non_writable_file.txt", 0400);
chmod("$dir/writable_file.txt", 0777);
include "$dir/non_writable_file.txt";
include "$dir/writable_file.txt";
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
Fatal error: [snuffleupagus][0.0.0.0][readonly_exec] Attempted execution of a writable file (%a/deny_writable_execution.php). in %a/deny_writable_execution.php on line 2
