--TEST--
Dump eval whitelist
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
sp.configuration_file={PWD}/config/dump_eval_whitelist.ini
--FILE--
<?php
@mkdir("/tmp/dump_result/");
foreach (glob("/tmp/dump_result/sp_dump.*") as $dump) {
    @unlink($dump);
}

function my_fun($p) {
	return "my_fun: $p";
}

function my_other_fun($p) {
	return "my_other_fun: $p";
}

$a = my_fun("1337 1337 1337");
echo "Outside of eval: $a\n";
eval('$a = my_fun("1234");');
echo "After allowed eval: $a\n";
eval('$a = my_other_fun("1234");');
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
Outside of eval: my_fun: 1337 1337 1337
After allowed eval: my_fun: 1234

Warning: [snuffleupagus][0.0.0.0][Eval_whitelist] The function 'my_other_fun' isn't in the eval whitelist, logging its call. in %a/dump_eval_whitelist.php on line 12
After eval: my_other_fun: 1234
