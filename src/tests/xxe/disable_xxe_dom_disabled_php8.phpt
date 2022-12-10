--TEST--
Disable XXE (feature disabled)
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || !extension_loaded("dom")) print("skip"); ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disable_xxe_disable.ini
--EXTENSIONS--
dom
--FILE--
<?php 
$dir = __DIR__;
// Just in case
@unlink($dir . "/content.xml");
@unlink($dir . "/content.txt");

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


$dom = new DOMDocument('1.0');
$dom->loadXML($xml, LIBXML_DTDATTR|LIBXML_DTDLOAD|LIBXML_NOENT);
printf("default setting with LIBXML_NOENT: %s\n", $dom->getElementsByTagName('testing')->item(0)->nodeValue);

$dom = new DOMDocument('1.0');
$dom->loadXML($xml, LIBXML_DTDATTR|LIBXML_DTDLOAD);
printf("default setting without LIBXML_NOENT: %s\n", $dom->getElementsByTagName('testing')->item(0)->nodeValue);

libxml_set_external_entity_loader(null);

$dom = new DOMDocument('1.0');
$dom->loadXML($xml, LIBXML_DTDATTR|LIBXML_DTDLOAD|LIBXML_NOENT);
printf("disabled entity loader with LIBXML_NOENT: %s\n", $dom->getElementsByTagName('testing')->item(0)->nodeValue);

$dom = new DOMDocument('1.0');
$dom->loadXML($xml, LIBXML_DTDATTR|LIBXML_DTDLOAD);
printf("disabled entity loader without LIBXML_NOENT: %s\n", $dom->getElementsByTagName('testing')->item(0)->nodeValue);

?>
--EXPECTF-- 
default setting with LIBXML_NOENT: WARNING, external entity loaded!
default setting without LIBXML_NOENT: 
disabled entity loader with LIBXML_NOENT: WARNING, external entity loaded!
disabled entity loader without LIBXML_NOENT:
