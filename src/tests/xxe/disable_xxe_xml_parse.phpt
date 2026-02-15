--TEST--
Disable XXE in xml_parse
--SKIPIF--
<?php
 if (!extension_loaded("snuffleupagus")) {
  echo "skip because snuffleupagus isn't loaded";
} elseif (!extension_loaded("xml")) {
  echo "skip because the `xml` extension isn't loaded";
}
 ?>
<?php if (PHP_VERSION_ID >= 80000) print "skip Not applicable for PHP 8+"; ?>
--EXTENSIONS--
xml
--INI--
sp.configuration_file={PWD}/config/disable_xxe.ini
--FILE--
<?php 
$dir = __DIR__;
@unlink($dir . "/content.xml");
@unlink($dir . "/content.txt");

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

function create_parser() {
	$parser = xml_parser_create();
	xml_set_element_handler(
		$parser,
		function($parser, $name, array $attributes) {
			var_dump($name);
			echo "\n";
			var_dump($attributes);
		},
		function($parser, $name) {
			var_dump($name);
		}
	);

	xml_set_character_data_handler(
		$parser,
		function ($parser, $text){
			echo 'text' . $text;
		}
	);

	return $parser;
}

libxml_disable_entity_loader(true);
$parser = create_parser();
$doc = xml_parse($parser, $xml, true);
xml_parser_free($parser);

libxml_disable_entity_loader(false);
$parser = create_parser();
$doc = xml_parse($parser, $xml, true);
xml_parser_free($parser);

$xml = "<test><testing>foo</testing></test>";
file_put_contents('content.xml', $xml);
$parser = create_parser();
$doc = xml_parse($parser, $xml, true);
xml_parser_free($parser);

--EXPECTF--
Warning: [snuffleupagus][0.0.0.0][xxe][log] A call to libxml_disable_entity_loader was tried and nopped in %a.php on line %d
string(4) "TEST"

array(0) {
}
string(7) "TESTING"

array(0) {
}
string(7) "TESTING"
string(4) "TEST"

Warning: [snuffleupagus][0.0.0.0][xxe][log] A call to libxml_disable_entity_loader was tried and nopped in %a.php on line %d
string(4) "TEST"

array(0) {
}
string(7) "TESTING"

array(0) {
}
string(7) "TESTING"
string(4) "TEST"
string(4) "TEST"

array(0) {
}
string(7) "TESTING"

array(0) {
}
textfoostring(7) "TESTING"
string(4) "TEST"
