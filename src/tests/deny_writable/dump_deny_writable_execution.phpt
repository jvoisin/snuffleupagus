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
--POST--
post_a=data_post_a_readonly&post_b=data_post_b_readonly
--GET--
get_a=data_get_a_readonly&get_b=data_get_b_readonly
--COOKIE--
cookie_a=data_cookie_a_readonly&cookie_b=data_cookie_b_readonly
--INI--
sp.configuration_file={PWD}/config/dump_deny_writable_execution.ini
--FILE--
<?php 
@mkdir("/tmp/dump_result/");
foreach (glob("/tmp/dump_result/sp_dump.*") as $dump) {
    @unlink($dump);
}
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

$filename = glob('/tmp/dump_result/sp_dump.*')[0];
$res = file($filename);
if ($res[2] != "GET:get_a='data_get_a_readonly' get_b='data_get_b_readonly' \n") {
    echo "1\n";
} elseif ($res[3] != "POST:post_a='data_post_a_readonly' post_b='data_post_b_readonly' \n") {
    echo "2\n";
} elseif ($res[4] != "COOKIE:cookie_a='data_cookie_a_readonly&cookie_b=data_cookie_b_readonly' \n") {
    echo "3\n";
} else {
	echo "WIN\n";
}
?>
--EXPECTF--
%a
WIN
