--TEST--
SPIP rules - CVE-2026-77647: metacharacter-free system() (X-Spip-Filtre RCE) is blocked
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
--INI--
sp.configuration_file={PWD}/../../../config/spip.rules
--FILE--
<?php
/* The X-Spip-Filtre chain `intval|_request|system` calls system() with an
   attacker-chosen, metacharacter-free argument, so it must be blocked outright. */
system("id");
echo "NOT-BLOCKED\n";
?>
--EXPECTF--
Fatal error: [snuffleupagus][0.0.0.0][disabled_function][drop] Aborted execution on call of the function 'system' in %a on line %d
