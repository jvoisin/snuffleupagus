Changelog
=========

0.11.0 - ` Mastodon <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.12.0>`__ 2024/09/06
-------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
* Compatibility with PHP8.4
* Print key and value on INI violations
* Improve scripts/generate_rules.php with regard to functions from global space prefixed with \
* Add option to specify the allowed "php" wrapper types

Breaking Changes
^^^^^^^^^^^^^^^^

Bug fixes
^^^^^^^^^
* Make 'phar' filenames work in sp.disabled_functions
* Improve the documentation
* Improve the default set of rules, especially with regard to portability
* Improve the Debian packaging
* Improve behaviour when dealing with broken configuration file
* Update the internal deprecation checks
* Don't whitelist files if the function name is actually a method of a class in scripts/generate_rules.php
* Ignore function definition in scripts/generate_rules.php
* Improve configuration dumping
* Fixed compilation on FreeBSD


0.10.0 - `Babar the Elephant <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.10.0>`__ 2023/09/20
----------------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
* Compatibility with PHP8.3
* Add `sp.log_max_len` to limit the maximum size of the log messages
* Add an example configuration for Xenforo 2.2.12 

Breaking Changes
^^^^^^^^^^^^^^^^
* Url encode functions arguments when logging them

Bug fixes
^^^^^^^^^
* Fix a possible NULL-byte truncation when outputting parameters in the logs
* Make readonly_exec play nice on readonly filesystems 


0.9.0 - `Elephant seal <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.9.0>`__ 2023/01/03
---------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
* Compatibility with PHP8.2
* Add the ability block object unserialization globally.


0.8.3 - `Elephant Gambit <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.8.3>`__ 2022/08/27
-----------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
* Add the ability to dump the parameter passed to `eval`
* Add the ability to match on `eval`'s parameter
* Add optional extended checks for `readonly_exec`
* Add config error for ini rules with identical key
* Add disabled functions return type to config export

Breaking Changes
^^^^^^^^^^^^^^^^
* Mix the stacktrace in the sha256 for the filename of .dump()

Bug fixes
^^^^^^^^^
* Make it actually possible to configure sloppy comparison on latests PHP7
* Allow file:// prefix in include() with readonly_exec mode
* Fix a possible crash when exporting function list
* Fix a minor memory leak when parsing cookie-related configuration


0.8.2 - `Surus <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.8.2>`__ 2022/05/20
-------------------------------------------------------------------------------------------

Bug fixes
^^^^^^^^^
* Fix compilation when ZTS is used
* Fix a possible infinite loop


0.8.1 - `Batyr <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.8.1>`__ 2022/05/16
-------------------------------------------------------------------------------------------

Bug fixes
^^^^^^^^^
* Fix the version number
* Fix a test on PHP7

Breaking Changes
^^^^^^^^^^^^^^^^
* `disable_xxe` is changed to `xxe_protection`


0.8.0 - `Woolly Mammoth <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.8.0>`__ 2022/05/15
-----------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
* Compatibility with PHP8.1
* Check for unsupported PHP version
* Backport of Suhosin-ng patches:

  * Maximum stack depth/recursion limit
  * Maximum length for session id
  * $_SERVER strip/encode
  * Configuration dump
  * Support for conditional rules
  * INI settings protection
  * Output SP logs to stderr
  * Ported Suhosin rules to SP

Improvements
^^^^^^^^^^^^
* Massive simplification of the configuration parser
* Better memory management
* Removal of internal calls to `call_user_func`
* Increased portability of the default rules access different version of PHP
* Start SP as late as possible, to hook as many things as possible

Bug fixes
^^^^^^^^^
* XML and Session support are now checked at runtime instead of at compile time


0.7.1 - `Proboscidea <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.7.0>`__ 2021/08/02
-------------------------------------------------------------------------------------------------

Improvements
^^^^^^^^^^^^
* Improve compatibility with various `libpcre` configurations/versions           
* Modernise the code by removing usage of `strtok`                               
* Improve the default rules' compatibility with php8                             
* Prevent XXE in php8 as well                                                    
* Improve a bit the verbosity of the logs
* Add a rules file for php8

Bug fixes
^^^^^^^^^
* Prevent a possible crash during configuration reloading                        
* Fix the default rules to catch dangerous `chmod` calls                         
* Fixed possible memory-leaks when hooking via regular expressions               


0.7.0 - `Los Elefantes <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.7.0>`__ 2021/01/02
---------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
* PHP8 support
* Stacktraces in dumps
* The ``>`` operator now skips over functions

Improvements
^^^^^^^^^^^^
* Move the CI from travis to gitlab-ci
* Some code simplifications and constifications
* PCRE2 is now used when possible
* The ``generate_rules.php`` script is now more portable

Bug fixes
^^^^^^^^^
* The strict mode can now be disabled


0.6.0 - `Elephant in the room <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.6.0>`__ 2020/11/06
----------------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
* Allow empty configurations

Improvements
^^^^^^^^^^^^

* More constification
* Snuffleupagus should now be able to get client's ip addresses in more cases
* Documented compatibility with Heroku
* Improved logging
* Added a couple of tests


0.5.1 - `Order of the Elephant <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.5.1>`__ 2020/06/20
-----------------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^
* Add support for syslog


Improvements
^^^^^^^^^^^^
* Improve OSX support
* Improve marginally of php8+ compatibility
* Improve php7.4 compatibility
* Improve the default ruleset
* Improve the documentation
* Improve the gitlab CI


0.5.0 - `Elephant Flats <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.5.0>`__ 2019/06/12
----------------------------------------------------------------------------------------------------

Improvements
^^^^^^^^^^^^

- Tighten a bit a command-injection prevention rule in the default rules set
- Increased the portability of the testsuite
- Improved documentation
- Usual code cleanup
- Snuffleupagus will throw an informative error when compiled for PHP5
- Snuffleupagus will throw an informative error when compiled without PCRE support
- The testsuite is now run on Alpine, Fedora, Debian and Ubuntu.
- Some rules against now-known vulnerabilities/techniques were added


Bug fixes
^^^^^^^^^

- PHP7.4 is fully supported, without any compilation warning
- Snuffleupagus can now be used with PHP compiled without sessions support as a builtin (which is the case on Alpine).
- Fix a compilation warning on FreeBSD
- Cookies hardening is now supported on PHP7.3+



0.4.1 - `Loxodonta <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.4.1>`__ 2018/12/21
-----------------------------------------------------------------------------------------------

Improvements
^^^^^^^^^^^^

- Improve and clarify the documentation
- Add support for PHP7.3
- Improve the coverage, we have reached 99% of coverage
- Improve `mb_string` hooking logic
- The script that check uploaded file is now available in PHP


Bug fixes
^^^^^^^^^

- Fix segfault on 32-bit for PHP7.3
- Fix segfault when using `sloppy_comparison` feature with array



0.4.0 - `Oliphant Chuckerbutty <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.4.0>`__ 2018/08/31
-----------------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^

- Add the possibility to whitelist `stream
  wrappers <https://secure.php.net/manual/en/intro.stream.php>`__
- Snuffleupagus is now using php's logging mechanisms, instead of 
  outputting its log directly into the syslog.
- PHP is now prevented from ever disabling certificate verification
  thanks to a few lines in our default configuration.


Improvements
^^^^^^^^^^^^

- Significant code simplification for cookies handling
  thanks to `Remi Collet <http://famillecollet.com>`__
- Our ``sloppy comparison`` feature is now complete
- Snuffleupagus won't start with an invalid config anymore,
  except if the ``sp.allow_broken_configuration`` is set.
- It's now possible to place virtual-patches on the return value
  of user-defined functions.
- Since Snuffleupagus is used by more and more organisations,
  we added a bunch of them in our propaganda page.

Bug fixes
^^^^^^^^^

- Add some missing pieces of documentation and fix some links
- Fix the ``make install`` command
- Fix various compilation warnings
- Snuffleupagus is now running on platforms that aren't using
  the glibc, thanks to an external contributor `Antoine Tenart
  <https://ack.tf>`__



0.3.1 - `Elephant Arch <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.3.1>`__ 2018/08/20
---------------------------------------------------------------------------------------------------

Improvements
^^^^^^^^^^^^

- Disable XXE and harden PRNG by default
- Use ``SameSite`` on PHP's session cookie in the default rules
- Relax a bit what files can be included in the default rules  
- Add the possibility to ignore files hashes when generating rules
- The ``filename`` filter is now accepting phar paths  

Bug fixes
^^^^^^^^^

- The harden rand_feature is not ignoring parameters anymore in function calls
- Fix possible crashes/hangs when using php-fpm's pools  
- Fix an infinite loop on ``echo`` hook
- Fix an issue with ``filename`` filter
- Fix some documentation issues
- Fix the Arch Linux's PKGBUILD


0.3.0 - `Dentalium elephantinum <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.3.0>`__ 2018/07/17
------------------------------------------------------------------------------------------------------------

New features
^^^^^^^^^^^^

- Session cookies can now be `encrypted <https://github.com/jvoisin/snuffleupagus/pull/178>`__
- Some occurrences of `type juggling <https://github.com/jvoisin/snuffleupagus/pull/186>`__ can now be eradicated
- It's  `now possible <https://github.com/jvoisin/snuffleupagus/pull/187>`__ to hook `echo` and `print`

Improvements
^^^^^^^^^^^^

- The `.filename()` filter is `now matching <https://github.com/jvoisin/snuffleupagus/pull/167>`__ on the file where the function is called instead on the one where it's defined.
- Vastly `optimize <https://github.com/jvoisin/snuffleupagus/issues/166>`__ the way we hook native functions
- The format of the logs has been streamlined to ease their processing


Bug fixes
^^^^^^^^^

- Better handling of filters for built-in functions
- Fix various possible integer overflows
- Fix an `annoying memory leak <https://github.com/jvoisin/snuffleupagus/issues/192#issuecomment-404538124>`__ impacting mostly `mod_php`  


0.2.2 - `Elephant Moraine <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.2.2>`__ 2018/04/12
------------------------------------------------------------------------------------------------------

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


0.2.1 - `Elephant Point <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.2.1>`__ 2018/02/07
----------------------------------------------------------------------------------------------------

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

0.2.0 - `Elephant Rally <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.2.0>`__ - 2018/01/18
------------------------------------------------------------------------------------------------------

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

0.1.0 - `Mighty Mammoth <https://github.com/jvoisin/snuffleupagus/releases/tag/v0.1.0>`__ - 2017/12/21
------------------------------------------------------------------------------------------------------

- Initial release
