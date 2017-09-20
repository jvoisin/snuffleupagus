--TEST--
Configuration line with too many quotes
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_lots_of_quotes.ini
--FILE--
--EXPECT--
[snuffleupagus][0.0.0.0][error][error] There is an issue with the parsing of '"this\"is a weird\"\"\"cookie\"name"");': it doesn't look like a valid string.
