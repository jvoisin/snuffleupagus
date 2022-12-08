--TEST--
Dump request
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) {
    print "skip";
} 
?>
--CLEAN--
<?php
if (is_dir("/tmp/dump_result/")) {
  foreach (glob("/tmp/dump_result/sp_dump.*") as $dump) {
      @unlink($dump);
  }
  @rmdir("/tmp/dump_result/");
}
?>
--POST--
post_a=data_post_a&post_b=data_post_b
--GET--
get_a=data_get_a&get_b=data_get_b
--COOKIE--
cookie_a=data_cookie_a&cookie_b=data_cookie_b
--INI--
sp.configuration_file={PWD}/config/dump_request.ini
--FILE--
<?php
@mkdir("/tmp/dump_result/");
foreach (glob("/tmp/dump_result/sp_dump.*") as $dump) {
    @unlink($dump);
}
echo "1\n";
function a(){ echo "a"; }
function b(){ a(); }
function c(){ b(); }
function d(){ c(); }
d();

$filename = glob('/tmp/dump_result/sp_dump.*')[0];
$res = file($filename);
//var_dump($res) . "\n";
if ($res[2] != "STACKTRACE: a:7\n") {
    echo "Invalid STACKTRACE for a";
} elseif ($res[3] != "STACKTRACE: b:8\n") {
    echo "Invalid STACKTRACE for b";
} elseif ($res[4] != "STACKTRACE: c:9\n") {
    echo "Invalid STACKTRACE for c";
} elseif ($res[5] != "STACKTRACE: d:10\n") {
    echo "Invalid STACKTRACE for d";
} elseif ($res[6] != "GET:get_a='data_get_a' get_b='data_get_b' \n") {
    echo "Invalid GET";
} elseif ($res[7] != "POST:post_a='data_post_a' post_b='data_post_b' \n") {
    echo "Invalid POST";
} elseif ($res[8] != "COOKIE:cookie_a='data_cookie_a&cookie_b=data_cookie_b' \n") {
    echo "Invalid COOKIE";
}
?>
--EXPECTF--
1

Warning: [snuffleupagus][0.0.0.0][disabled_function][simulation] Aborted execution on call of the function 'a' in %a/dump_request_stacktrace.php on line 7
a
