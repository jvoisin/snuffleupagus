Changelog
=========

0.2.2 - `Elephant Moraine <https://github.com/nbs-system/snuffleupagus/releases/tag/v0.2.2>`__ 2018/04/12
---------------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
- The `.dump()` filter is now supported for `unserialize`, `readonly_exec`, and `eval` black/whitelist

Improvements
^^^^^^^^^^^^

- Add some assertions
- Add more rules examples
- Provide a script to check for malicious file uploads
- Significant performances improvement (at least +20%)
- Significantly improve the performances of our default rules set
- Our readme file is now shinier
- Minor code simplification

Bug fixes
^^^^^^^^^
- Fix a crash related to variadic functions


0.2.1 - `Elephant Point <https://github.com/nbs-system/snuffleupagus/releases/tag/v0.2.1>`__ 2018/02/07
-------------------------------------------------------------------------------------------------------

Bug fixes
^^^^^^^^^

- The testsuite can now be successfully run as root
- Fix a double execution when snuffleupagus is used with some other extensions
- Fix an execution-context related crash

Improvements
^^^^^^^^^^^^

- Support PCRE2, since it's `required for PHP7.3 <https://wiki.php.net/rfc/pcre2-migration>`__
- Improve a bit the portability of the code
- Minor code simplification

0.2.0 - `Elephant Rally <https://github.com/nbs-system/snuffleupagus/releases/tag/v0.2.0>`__ - 2018/01/18
---------------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^

- `Glob <https://en.wikipedia.org/wiki/Glob_%28programming%29>`__ support in ``sp.configuration_file``
- Whitelist/blacklist functions in ``eval``
- ``phpinfo`` shows if the configuration is valid or not

Bug fixes
^^^^^^^^^

- Off-by-one in configuration parsing fixed
- Minor cookie-encryption related memory leaks fixes
- Various crashes spotted by `fr33tux <https://fr33tux.org/>`__ fixes
- Configuration files with windows EOL are correctly handled

Improvements
^^^^^^^^^^^^

- General code clean-up
- Documentation overhaul
- Compilation on FreeBSD and CentOS
- Select which cookies to encrypt via regular expressions
- Match on return values from user-defined functions

External contributions
^^^^^^^^^^^^^^^^^^^^^^

- Simplification and clean up of our linked-list implementation by `smagnin <https://github.com/smagnin>`__

0.1.0 - `Mighty Mammoth <https://github.com/nbs-system/snuffleupagus/releases/tag/v0.1.0>`__ - 2017/12/21
---------------------------------------------------------------------------------------------------------

- Initial release
