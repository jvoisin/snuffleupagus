--TEST--
Dump eval blacklist
--SKIPIF--
<?php 
if (!extension_loaded("snuffleupagus")) die "skip";
?>
--POST--
post_a=data_post_a&post_b=data_post_b
--GET--
get_a=data_get_a&get_b=data_get_b
--COOKIE--
cookie_a=data_cookie_a&cookie_b=data_cookie_b
--INI--
sp.configuration_file={PWD}/config/dump_eval_blacklist.ini
--FILE--
<?php
@mkdir("/tmp/dump_result/");
foreach (glob("/tmp/dump_result/sp_dump.*") as $dump) {
    @unlink($dump);
}

$a = strlen("1337 1337 1337");
echo "Outside of eval: $a\n";
eval('$a = strlen("1234");');
echo "After eval: $a\n";
$filename = glob('/tmp/dump_result/sp_dump.*')[0];
$res = file($filename);
if ($res[2] != "GET:get_a='data_get_a' get_b='data_get_b' \n") {
    echo "1\n";
} elseif ($res[3] != "POST:post_a='data_post_a' post_b='data_post_b' \n") {
    echo "2\n";
} elseif ($res[4] != "COOKIE:cookie_a='data_cookie_a&cookie_b=data_cookie_b' \n") {
    echo "3\n";
}
?>
--EXPECTF--
Outside of eval: 14

Warning: [snuffleupagus][eval] A call to strlen was tried in eval, in %a/dump_eval_blacklist.php:1, logging it. in %a/dump_eval_blacklist.php(9) : eval()'d code on line 1
After eval: 4
