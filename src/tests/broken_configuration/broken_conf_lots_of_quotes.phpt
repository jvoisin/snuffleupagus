--TEST--
Configuration line with too many quotes
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/broken_conf_lots_of_quotes.ini
--FILE--
--EXPECT--
PHP Fatal error:  [snuffleupagus][0.0.0.0][error][log] There is an issue with the parsing of '"this\"is a weird\"\"\"cookie\"name"");': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][error][log] There is an issue with the parsing of '"this\"is a weird\"\"\"cookie\"name"");': it doesn't look like a valid string on line 1 in Unknown on line 0

Fatal error: [snuffleupagus][0.0.0.0][config][log] Invalid configuration file in Unknown on line 0
Could not startup.