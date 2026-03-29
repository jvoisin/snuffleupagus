--TEST--
Test idempotence of serialize+unserialize
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_serialize.ini
--FILE--
<?php 
class A {
public $pub = "public data";
protected $prot = "protected data";
private $priv = "private data";
}
$a = new A;
if (unserialize(serialize($a)) == $a) {
	echo "OK";
} else {
	echo "FAIL";
}
?>
--EXPECT--
OK

