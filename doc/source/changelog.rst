Changelog
=========

0.2.0 - `Elephant Rally <https://github.com/nbs-system/snuffleupagus/releases/tag/v0.3.0>`__ - 2018/01/18
---------------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^

- `Glob <https://en.wikipedia.org/wiki/Glob_%28programming%29>`__ support in ``sp.configuration_file``
- Whitelist/blacklist functions in ``eval``
- ``phpinfo`` shows is the configuration is valid or not

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
