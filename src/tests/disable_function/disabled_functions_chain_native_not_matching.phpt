--TEST--
Disable functions by matching the calltrace, ending on a native function, not matching
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_chain_native.ini
--FILE--
<?php

function other_wrapper($data) {
    return base64_decode($data);
}

echo "direct: " . base64_decode("c25hZmZsZQ==") . "\n";
echo "wrapped: " . other_wrapper("c25hZmZsZQ==") . "\n";
?>
--EXPECTF--
direct: snaffle
wrapped: snaffle
