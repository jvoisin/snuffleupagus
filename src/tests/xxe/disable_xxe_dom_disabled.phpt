--TEST--
Disable XXE
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || !extension_loaded("dom")) print("skip"); ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disable_xxe_disable.ini
--EXTENSIONS--
dom
--FILE--
<?php 
$dir = __DIR__;
$content = '<content>WARNING, external entity loaded!</content>';
file_put_contents($dir . '/content.txt', $content);

$xml = <<<EOD
<?xml version="1.0"?>
<!DOCTYPE root
[
<!ENTITY foo SYSTEM "file://$dir/content.txt">
]>
<test><testing>&foo;</testing></test>
EOD;

file_put_contents($dir . '/content.xml', $xml);

libxml_disable_entity_loader(true);
$dom = new DOMDocument('1.0');
$dom->loadXML($xml, LIBXML_DTDATTR|LIBXML_DTDLOAD|LIBXML_NOENT);
printf("libxml_disable_entity to true: %s\n", $dom->getElementsByTagName('testing')->item(0)->nodeValue);

libxml_disable_entity_loader(false);
$dom = new DOMDocument('1.0');
$dom->loadXML($xml, LIBXML_DTDATTR|LIBXML_DTDLOAD|LIBXML_NOENT);
printf("libxml_disable_entity to false: %s\n", $dom->getElementsByTagName('testing')->item(0)->nodeValue);

$xml = "<test><testing>foo</testing></test>";
file_put_contents('content.xml', $xml);

libxml_disable_entity_loader(false);
$dom = new DOMDocument('1.0');
$dom->loadXML($xml, LIBXML_DTDATTR|LIBXML_DTDLOAD|LIBXML_NOENT);
printf("without xxe: %s", $dom->getElementsByTagName('testing')->item(0)->nodeValue);

?>
--EXPECTF-- 
libxml_disable_entity to true: WARNING, external entity loaded!
libxml_disable_entity to false: WARNING, external entity loaded!
without xxe: foo
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . "/content.xml");
unlink($dir . "/content.txt");
?>
