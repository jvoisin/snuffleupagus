--TEST--
Cookie encryption key too short
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (PHP_VERSION_ID < 80000) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../broken_configuration/config/config_encryption_key_short.ini
--COOKIE--
--ENV--
return <<<EOF
REMOTE_ADDR=2001:0db8:0000:0000:0000:fe00:0042:8329
HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/59.0.3071.109 Chrome/59.0.3071.109 Safari/537.36
HTTPS=1
EOF;
--FILE--
<?php
?>
--EXPECT--
Fatal error: [snuffleupagus][2001:0db8:0000:0000:0000:fe00:0042:8329][config][log] The encryption key set on line 1 is too short. please use at least 10 bytes in Unknown on line 0

Fatal error: [snuffleupagus][2001:0db8:0000:0000:0000:fe00:0042:8329][config][log] Invalid configuration file in Unknown on line 0
Could not startup.