--TEST--
Dump eval blacklist
--SKIPIF--
<?php 
if (!extension_loaded("snuffleupagus")) print "skip";
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

$a = strtoupper("1337 1337 1337");
echo "Outside of eval: $a\n";
eval('$a = strtoupper("1234");');
echo "After eval: $a\n";
$filename = glob('/tmp/dump_result/sp_dump.*')[0];
$res = file($filename);
if ($res[3] != "GET:get_a='data_get_a' get_b='data_get_b' \n") {
    echo "Invalid GET";
} elseif ($res[4] != "POST:post_a='data_post_a' post_b='data_post_b' \n") {
    echo "Invalid POST";
} elseif ($res[5] != "COOKIE:cookie_a='data_cookie_a&cookie_b=data_cookie_b' \n") {
    echo "Invalid COOKIE";
}
?>
--EXPECTF--
Outside of eval: 1337 1337 1337

Warning: [snuffleupagus][0.0.0.0][eval][simulation] A call to 'strtoupper' was tried in eval. logging it. in %a(9) : eval()'d code on line 1
After eval: 1234
