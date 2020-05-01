--TEST--
Disable XXE
--SKIPIF--
<?php
 if (!extension_loaded("snuffleupagus")) {
  echo "skip"; 
} elseif (!extension_loaded("dom")) {
	echo "skip";
}
 ?>
--EXTENSIONS--
dom
--INI--
sp.configuration_file={PWD}/config/disable_xxe.ini
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
--EXPECTF--
Warning: DOMDocument::loadXML(): I/O warning : failed to load external entity "file://%a/content.txt" in %a/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): Failure to process entity foo in Entity, line: %d in %a/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): Entity 'foo' not defined in Entity, line: %d in %a/disable_xxe_dom.php on line %d

Notice: Trying to get property %a in %a/disable_xxe_dom.php on line %d
libxml_disable_entity to true: 

Warning: DOMDocument::loadXML(): I/O warning : failed to load external entity "file://%a/content.txt" in %a/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): Failure to process entity foo in Entity, line: %d in %a/disable_xxe_dom.php on line %d

Warning: DOMDocument::loadXML(): Entity 'foo' not defined in Entity, line: %d in %a/disable_xxe_dom.php on line %d

Notice: Trying to get property %a in %a/disable_xxe_dom.php on line %d
libxml_disable_entity to false: 
without xxe: foo
--CLEAN--
<?php
$dir = __DIR__;
unlink($dir . "content.xml");
unlink($dir . "content.txt");
?>
