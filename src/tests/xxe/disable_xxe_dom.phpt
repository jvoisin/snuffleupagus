--TEST--
Disable XXE, in php8
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus") || !extension_loaded("dom")) print("skip"); ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/disable_xxe.ini
--EXTENSIONS--
dom
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
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . "content.xml");
unlink($dir . "content.txt");
?>
--EXPECTF--
Deprecated: Function libxml_disable_entity_loader() is deprecated in %s/tests/xxe/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): I/O warning : failed to load external entity "file://%s/tests/xxe/content.txt" in /var/www/html/snuffleupagus/src/tests/xxe/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): Failure to process entity foo in Entity, line: 6 in %s/tests/xxe/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): Entity 'foo' not defined in Entity, line: 6 in %s/tests/xxe/disable_xxe_dom.php on line %d

Warning: Attempt to read property "nodeValue" on null in %s/tests/xxe/disable_xxe_dom.php on line %d
libxml_disable_entity to true: 

Deprecated: Function libxml_disable_entity_loader() is deprecated in %s/tests/xxe/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): I/O warning : failed to load external entity "file://%s/tests/xxe/content.txt" in /var/www/html/snuffleupagus/src/tests/xxe/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): Failure to process entity foo in Entity, line: 6 in %s/tests/xxe/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): Entity 'foo' not defined in Entity, line: 6 in %s/tests/xxe/disable_xxe_dom.php on line %d

Warning: Attempt to read property "nodeValue" on null in %s/tests/xxe/disable_xxe_dom.php on line %d
libxml_disable_entity to false: 

Deprecated: Function libxml_disable_entity_loader() is deprecated in %s/tests/xxe/disable_xxe_dom.php on line %d
