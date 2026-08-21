--TEST--
Disable functions by matching the calltrace, ending on a native function
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_chain_native.ini
--FILE--
<?php

function mywrapper($data) {
    return base64_decode($data);
}

echo "I'm before the call to mywrapper\n";
mywrapper("c25hZmZsZQ==");
echo "I'm after the call to mywrapper\n";
?>
--EXPECTF--
I'm before the call to mywrapper

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'mywrapper>base64_decode' in %a/disabled_functions_chain_native.php on line 4
