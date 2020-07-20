--TEST--
Dump unserialize
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) {
    print "skip";
} 
?>
--POST--
post_a=data_post_a&post_b=data_post_b
--GET--
get_a=data_get_a&get_b=data_get_b
--COOKIE--
cookie_a=data_cookie_a&cookie_b=data_cookie_b
--INI--
sp.configuration_file={PWD}/config/dump_unserialize.ini
--FILE--
<?php
@mkdir("/tmp/dump_result/");
foreach (glob("/tmp/dump_result/sp_dump.*") as $dump) {
    @unlink($dump);
}

echo "1\n";
var_dump(unserialize('s:1:"a";alyualskdufyhalkdjsfhalkjdhflaksjdfhlkasdhflkahdawkuerylksjdfhlkssjgdflaksjdhflkasjdf'));
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
1

Fatal error: [snuffleupagus][0.0.0.0][unserialize][drop] Invalid HMAC for s:1:"a";alyualskdufyhalkdjsfh in %a/dump_unserialize.php on line 8
