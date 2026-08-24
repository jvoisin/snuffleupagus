--TEST--
SPIP rules - CVE-2024-7954: eval_blacklist blocks code-exec helpers called inside eval()
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../../config/spip.rules
--FILE--
<?php
/* Obfuscated (variable-function) call: eval_blacklist matches the runtime call,
   not the source, so this is caught even though "system" is built at runtime. */
eval('$f = "sys" . "tem"; $f("id");');
echo "NOT-BLOCKED\n";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][eval][drop] A call to 'system' was tried in eval. dropping it. in %a on line %d
