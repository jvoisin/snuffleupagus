--TEST--
Readonly execution attempt
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disable_writable_disabled.ini
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
--EXPECT--
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
