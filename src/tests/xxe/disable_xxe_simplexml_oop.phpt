--TEST--
Disable XXE
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || !extension_loaded("simplexml") || getenv('TRAVIS')) print("skip"); ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disable_xxe_disable.ini
--EXTENSIONS--
simplexml
--XFAIL--
--FILE--
<?php 
$dir = __DIR__;
$content = 'WARNING, external entity loaded!';
file_put_contents('content.txt', $content);

$xml = <<<EOD
<?xml version="1.0"?>
<!DOCTYPE root
[
<!ENTITY foo SYSTEM "file://$dir/content.txt">
]>
<test><testing>&foo;</testing></test>
EOD;

file_put_contents('content.xml', $xml);

libxml_disable_entity_loader(true);
$doc = simplexml_load_string($xml);
printf("libxml_disable_entity to true: %s\n", $doc->testing);

libxml_disable_entity_loader(false);
$doc = simplexml_load_string($xml);
printf("libxml_disable_entity to false: %s\n", $doc->testing);

$xml = "<test><testing>foo</testing></test>";
file_put_contents('content.xml', $xml);

$doc = simplexml_load_string($xml);
printf("without xxe: %s", $doc->testing);

?>
--EXPECT--
libxml_disable_entity to true: 
libxml_disable_entity to false: 
without xxe: foo
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . "/content.xml");
unlink($dir . "/content.txt");
?>
