--TEST--
Readonly execution attempt (simulation mode)
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) print "skip";

$filename = __DIR__ . '/test.txt';

@unlink($filename);

file_put_contents($filename, 'a');
chmod($filename, 0400);

if (is_writable($filename)) print "skip";;
@unlink($filename);
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
--EXPECTF--
[snuffleupagus][0.0.0.0][readonly_exec][simulation] Attempted execution of a writable file (%a/writable_file.txt).
Code execution within a writable file.
Code execution within a non-writable file.
--CLEAN--
<?php
$dir = __DIR__;
chmod("$dir/non_writable_file.txt", 0777);
chmod("$dir/writable_file.txt", 0777);
unlink("$dir/non_writable_file.txt");
unlink("$dir/writable_file.txt");
?>
