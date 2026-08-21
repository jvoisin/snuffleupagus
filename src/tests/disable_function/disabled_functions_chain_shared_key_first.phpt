--TEST--
Two chains sharing the same last function: the first one still matches
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_disabled_functions_chain_shared_key.ini
--FILE--
<?php

function wrapper_a($data) {
    return base64_decode($data);
}

function wrapper_b($data) {
    return base64_decode($data);
}

echo "direct: " . base64_decode("c25hZmZsZQ==") . "\n";
wrapper_a("c25hZmZsZQ==");
echo "unreached\n";
?>
--EXPECTF--
direct: snaffle

Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'wrapper_a>base64_decode' in %a/disabled_functions_chain_shared_key_first.php on line 4
