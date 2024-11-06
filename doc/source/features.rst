Features
========

Snuffleupagus has a lot of features that can be divided in two main categories: bug-classes
killers and virtual-patching. The first category provides primitives to kill various
bug families (like arbitrary code execution via ``unserialize`` for example) or raise the
cost of exploitation. The second category is a highly configurable system to patch functions in php itself.

Bug classes killed or mitigated
-------------------------------

``system`` injections
^^^^^^^^^^^^^^^^^^^^^

The ``system`` function executes an external program and displays the output.
It is used to interact with various external tools, like file-format converters.
Unfortunately, passing user-controlled parameters to it often leads to arbitrary command execution.

  When allowing user-supplied data to be passed to this function,
  use `escapeshellarg()` or `escapeshellcmd()` to ensure that users cannot trick
  the system into executing arbitrary commands.

  --- `The PHP documentation about system <https://secure.php.net/manual/en/function.system.php>`_

We're mitigating it by filtering the ``$``, ``|``, ``;``, `````, ``\n`` and ``&`` chars in our
default configuration, making it a lot harder for an attacker to inject arbitrary commands.
This feature is even more effective when used along with :ref:`readonly_exec <readonly-exec-feature>`.


Examples of related vulnerabilities
"""""""""""""""""""""""""""""""""""

- `CVE-2013-3630 <https://www.rapid7.com/db/modules/exploit/multi/http/moodle_cmd_exec>`__: Authenticated remote code execution in Moodle
- `CVE-2014-1610 <https://www.rapid7.com/db/modules/exploit/multi/http/mediawiki_thumb>`__: Unauthenticated remote code execution in DokuWiki
- `CVE-2014-4688 <https://www.pfsense.org/security/advisories/pfSense-SA-14_10.webgui.asc>`__: Authenticated remote code execution in pfSense
- `CVE-2017-7981 <https://tuleap.net/plugins/tracker/?aid=10159>`__: Authenticated remote code execution in Tuleap
- `CVE-2018-20434 <https://www.exploit-db.com/exploits/47044>`__: Authenticated remote code execution in LibreNMS
- `CVE-2020-5791 <https://www.tenable.com/security/research/tra-2020-58)>`__: Authenticated remote code execution in Nagios XI
- `CVE-2020-8813 <https://www.exploit-db.com/exploits/48159>`__: Unauthenticated remote code execution in Cacti
- Every single `modem/router/switch/IoT/… <https://twitter.com/internetofshit>`_.


``mail``-related injections
^^^^^^^^^^^^^^^^^^^^^^^^^^^

This vulnerability has been known `since 2011 <http://esec-pentest.sogeti.com/posts/2011/11/03/using-mail-for-remote-code-execution.html>`_
and was popularized by `RIPS <https://www.ripstech.com/blog/2016/roundcube-command-execution-via-email/>`_ in 2016.
The last flag of the `mail` function can be used to pass various parameters to
the underlying binary used to send emails; this can lead to an arbitrary file write,
often meaning an arbitrary code execution.

  The ``additional_parameters`` parameter can be used to pass additional flags
  as command line options to the program configured to be used when sending mail

  --- `The PHP documentation about mail <https://secure.php.net/manual/en/function.mail.php>`_

We're killing it by preventing any extra options in ``additional_parameters``.
This feature is even more effective when used along with :ref:`readonly_exec <readonly-exec-feature>`.


Examples of related vulnerabilities
"""""""""""""""""""""""""""""""""""

- `CVE-2017-7692 <https://legalhackers.com/advisories/SquirrelMail-Exploit-Remote-Code-Exec-CVE-2017-7692-Vuln.html>`_: Authenticated remote code execution in SquirrelMail
- `CVE-2016-10074 <https://legalhackers.com/advisories/SwiftMailer-Exploit-Remote-Code-Exec-CVE-2016-10074-Vuln.html>`_: remote code execution in SwiftMailer
- `CVE-2016-10033 <https://legalhackers.com/advisories/PHPMailer-Exploit-Remote-Code-Exec-CVE-2016-10033-Vuln.html>`_: remote code execution in PHPMailer
- `CVE-2016-9920 <https://www.ripstech.com/blog/2016/roundcube-command-execution-via-email/>`_: Unauthenticated remote code execution in Roundcube
- `CVE-2019-???? <https://www.exploit-db.com/exploits/46136>`__:  Unauthenticated remote code execution in Horde

.. _cookie-encryption-feature:

Cookie stealing via XSS
^^^^^^^^^^^^^^^^^^^^^^^

The goto payload for XSS is often to steal cookies.
Like *Suhosin*, we are encrypting the cookies with a secret key,
an environment variable (usually the IP of the user)
and the user's user-agent. This means that an attacker with an XSS won't be able to use
the stolen cookie, since he can't spoof the content of the value of the environment
variable for the user. Please do read the :ref:`documentation about this feature <cookie-encryption-page>`
if you're planning to use it.

This feature is roughly the same than the `Suhosin one <https://suhosin.org/stories/configuration.html#transparent-encryption-options>`_.

Having a secret server-side key will prevent anyone (even the user)
from reading the content of the cookie, reducing the impact of an application storing sensitive data client-side.


.. _fileupload-feature:

Remote code execution via file-upload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some PHP applications allows users to upload content like avatars to a forum.
Unfortunately, content validation often isn't implemented properly (if at all),
meaning arbitrary file upload often leads to an arbitrary code execution, contrary to the documentation.

  Not validating which file you operate on may mean that users can *access sensitive information* in other directories.

  --- `The PHP documentation about file uploads <https://secure.php.net/manual/en/features.file-upload.common-pitfalls.php>`_

We're killing it, like Suhosin, by automatically calling a script upon file upload,
if it returns something else than ``0``, the file will be removed (or stored in a quarantine,
for further analysis).

We're recommending to use the `vld <https://derickrethans.nl/projects.html#vld>`_ project
inside the script to ensure the file doesn't contain any valid PHP code, with something like this:

::

  $ php -d vld.execute=0 -d vld.active=1 -d extension=vld.so $file

One could also filter on the file extensions, with something like this:

::

  #!/bin/bash
  exit $([[ $SP_FILENAME =~ *\.php* ]])


Examples of related vulnerabilities
"""""""""""""""""""""""""""""""""""

- `CVE-2017-6090 <https://sysdream.com/news/lab/2017-09-29-cve-2017-6090-phpcollab-2-5-1-arbitrary-file-upload-unauthenticated/>`_: Unauthenticated remote code execution in PhpCollab
- `EDB-38407 <https://www.exploit-db.com/exploits/38407/>`_: Authenticated remote code execution in GLPI
- `CVE-2013-5576 <https://developer.joomla.org/security/news/563-20130801-core-unauthorised-uploads>`_: Authenticated remote code execution in Joomla
- `CVE-2019-15813 <https://www.exploit-db.com/exploits/48955>`__: Authenticated remote code execution in Sentrifugo
- `CVE-2019-17132 <http://karmainsecurity.com/KIS-2019-02>`__: Authenticated remote code execution in vBulletin
- `CVE-2020-10682 <http://dev.cmsmadesimple.org/bug/view/12275>`__: Authenticated remote code execution in CMS Made Simple
- `EDB-19154 <https://www.rapid7.com/db/modules/exploit/multi/http/qdpm_upload_exec>`_: Authenticated remote code execution in qdPM


.. _unserialize-feature:

Unserialize-related magic
^^^^^^^^^^^^^^^^^^^^^^^^^

PHP is able to *serialize* arbitrary objects, to easily store them.
Unfortunately, as demonstrated by `Stefan Esser <https://twitter.com/i0n1c>`__ in his `Shocking News in PHP Exploitation <https://www.owasp.org/images/f/f6/POC2009-ShockingNewsInPHPExploitation.pdf>`__ and `Utilizing Code Reuse/ROP in PHP
Application Exploits <https://www.owasp.org/images/9/9e/Utilizing-Code-Reuse-Or-Return-Oriented-Programming-In-PHP-Application-Exploits.pdf>`__ slides, it is often possible to gain arbitrary code execution upon deserialization
of user-supplied serialized objects.

  Do not pass untrusted user input to ``unserialize()`` regardless of the options value of allowed_classes.
  Unserialization can result in code being loaded and executed due to object instantiation and autoloading
  and a malicious user may be able to exploit this.

  --- `The PHP documentation about serialize <https://secure.php.net/manual/en/function.serialize.php>`_

We're killing it by exploiting the fact that PHP will discard any garbage found at the end of a serialized object,
allowing us to simply append a `HMAC <https://en.wikipedia.org/wiki/Hash-based_message_authentication_code>`_
at the end of strings generated by the ``serialize``,
hence guaranteeing that any object deserialized came from the application
and wasn't tampered with.

We aren't encrypting it, like we do with the cookies,
allowing this feature to be disabled (or switch into leaning mode)
without the need to invalidate any data.

.. warning::

    This feature can't be deployed on websites that already stored serialized
    objects (ie. in database), since they are missing the HMAC and thus will be detected as
    an attack. If you're in this situation, you should use this feature with the
    ``simulation`` mode, and switch it off once you don't have any messages in your
    logs.

A nice side-effect of this feature is that it will defeat various memory corruption
issues related to the complexity of ``unserialize``'s implementation,
and the amount of control if provides to an attacker, like `CVE-2016-9137,
CVE-2016-9138 <https://bugs.php.net/bug.php?id=73147>`_, `2016-7124
<https://bugs.php.net/bug.php?id=72663>`_, `CVE-2016-5771 and CVE-2016-5773
<https://www.evonide.com/how-we-broke-php-hacked-pornhub-and-earned-20000-dollar/>`_.

A less subtle mitigation can be used to simply prevent the deserialization of objects altogether.


Examples of related vulnerabilities
"""""""""""""""""""""""""""""""""""

- `CVE-2012-5692 <https://www.rapid7.com/db/modules/exploit/unix/webapp/invision_pboard_unserialize_exec>`_: Unauthenticated remote code execution in IP.Board
- `CVE-2014-1691 <http://seclists.org/oss-sec/2014/q1/153>`_: Unauthenticated remote code execution in Horde
- `CVE-2015-7808 <https://www.rapid7.com/db/modules/exploit/multi/http/vbulletin_unserialize>`_: Unauthenticated remote code execution in vBulletin
- `CVE-2015-8562 <https://www.rapid7.com/db/modules/exploit/multi/http/joomla_http_header_rce>`_: Unauthenticated remote code execution in Joomla
- `CVE-2016-4010 <http://netanelrub.in/2016/05/17/magento-unauthenticated-remote-code-execution/>`_: Unauthenticated remote code execution in Magento
- `CVE-2016-5726 <http://seclists.org/oss-sec/2016/q2/521>`_: Unauthenticated remote code execution in Simple Machines Forums
- `CVE-2016-???? <https://www.computest.nl/advisories/CT-2016-1110_Observium.txt>`_: Unauthenticated remote code execution in Observium (leading to remote root)
- `CVE-2017-2641 <http://netanelrub.in/2017/03/20/moodle-remote-code-execution/>`_: Unauthenticated remote code execution in Moodle
- `CVE-2018-17057 <https://www.exploit-db.com/exploits/46634>`__: Unauthenticated remote code execution in LimeSurvey
- `CVE-2018-19274 <https://blog.ripstech.com/2018/phpbb3-phar-deserialization-to-remote-code-execution/>`__: Authenticated remote code execution in phpBB
- `CVE-2019-6340 <https://www.ambionics.io/blog/drupal8-rce>`__:  Unauthenticated remote code execution in Drupal


.. _harden-rand-feature:

Weak-PRNG via rand/mt_rand
^^^^^^^^^^^^^^^^^^^^^^^^^^

The functions ``rand`` and ``mt_rand`` are often used to generate random numbers used
in sensitive context, like password generation, token creation.
Unfortunately, as stated in the documentation, the quality of their entropy is low,
leading to the generation of guessable values.

  This function does not generate cryptographically secure values, and should not be used for cryptographic purposes.

  --- `The PHP documentation about rand <https://secure.php.net/manual/en/function.rand.php>`_

We're addressing this issue by replacing every call to ``rand`` and ``mt_rand`` with
a call to the ``random_int``, a `CSPRNG <https://en.wikipedia.org/wiki/Cryptographically_secure_pseudorandom_number_generator>`_.

It's worth noting that the PHP documentation contains the following warning:

  ``min`` ``max`` range must be within the range ``getrandmax()``. i.e. ``(max - min) <= getrandmax()``.
  Otherwise, ``rand()`` may return poor-quality random numbers.

  --- `The PHP documentation about rand <https://secure.php.net/manual/en/function.rand.php>`_

This is of course addressed as well by the ``harden_rand`` feature.


Examples of related vulnerabilities
"""""""""""""""""""""""""""""""""""

- `CVE-2015-5267 <https://moodle.org/mod/forum/discuss.php?d=320291>`_: Unauthenticated accounts takeover in in Moodle
- `CVE-2014-9624 <https://www.mantisbt.org/bugs/view.php?id=17984>`_: Captcha bypass in MantisBT
- `CVE-2014-6412 <https://core.trac.wordpress.org/ticket/28633>`_: Unauthenticated account takeover in Wordpress
- `CVE-2015-???? <https://hackerone.com/reports/31171>`_: Unauthenticated accounts takeover in Concrete5
- `CVE-2013-6386 <https://www.drupal.org/SA-CORE-2013-003>`_: Unauthenticated accounts takeover in Drupal
- `CVE-2010-???? <http://www.sektioneins.com/advisories/advisory-022010-mybb-password-reset-weak-random-numbers-vulnerability.html>`_: Unauthenticated accounts takeover in MyBB
- `CVE-2008-4102 <https://sektioneins.de/en/advisories/advisory-042008-joomla-weak-random-password-reset-token-vulnerability.html>`_: Unauthenticated accounts takeover in Joomla
- `CVE-2006-0632 <https://www.cvedetails.com/cve/CVE-2006-0632/>`_: Unauthenticated account takeover in phpBB

.. _xxe-feature:

XXE
^^^

Despite the documentation saying nothing about this class of vulnerabilities,
`XML eXternal Entity <https://www.owasp.org/index.php/XML_External_Entity_(XXE)_Processing>`_  (XXE) often leads to arbitrary file reading, `SSRF <https://www.owasp.org/index.php/Server_Side_Request_Forgery>`_ and sometimes even arbitrary
code execution.

XML documents can contain a `Document Type Definition <https://www.w3.org/TR/REC-xml/#sec-prolog-dtd>`_ (DTD),
enabling definition of XML entities. It is possible to define an (external) entity by a
URI, that the parser will access and embed its content back into the document
for further processing.

For example, providing an url like ``file:///etc/passwd`` will read
the file's content. Since the file is not valid XML, the application
will spit it out in an error message, thus leaking its content.

We're killing this class of vulnerabilities by calling
the `libxml_disable_entity_loader <https://secure.php.net/manual/en/function.libxml-disable-entity-loader.php>`_
function with its parameter set to ``true`` at startup,
and then *nop'ing* it, so it won't do anything if ever called again.


Examples of related vulnerabilities
"""""""""""""""""""""""""""""""""""

- `CVE-2015-5161 <https://legalhackers.com/advisories/eBay-Magento-XXE-Injection-Vulnerability.html>`_: Unauthenticated arbitrary file disclosure on Magento
- `CVE-2014-8790 <https://github.com/GetSimpleCMS/GetSimpleCMS/issues/944>`_: Unauthenticated remote code execution in GetSimple CMS
- `CVE-2011-4107 <https://www.phpmyadmin.net/security/PMASA-2011-17/>`_: Authenticated local file disclosure in PHPMyAdmin


.. _auto-cookie-secure-feature:

Cookie stealing via HTTP MITM
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

While it's possible to set the ``secure`` flag on cookies to prevent them from being
transmitted over HTTP, and only allow its transmission over HTTPS.
Snuffleupagus can automatically set this flag if the client is accessing the
website over a secure connection.

This behaviour is suggested in the documentation:

   On the server-side, it's on the programmer to send this kind of cookie only
   on secure connection (e.g. with respect to ``$_SERVER["HTTPS"]``).

   --- `The PHP documentation about setcookie <https://secure.php.net/manual/en/function.setcookie.php>`_


Exploitation, post-exploitation and general hardening
-----------------------------------------------------


.. _virtual-patching-feature:

Virtual-patching
^^^^^^^^^^^^^^^^

PHP itself exposes a number of functions that might be considered **dangerous** and that have limited legitimate use cases.
``system()``, ``exec()``, ``dlopen()`` - for example - fall into this category. By default, PHP only allows us to globally disable some functions.


However, (ie. ``system()``) they might have legitimate use cases in processes such as self upgrade etc., making it impossible to effectively
disable them - at the risk of breaking critical features.

Snuffleupagus allows the user to restrict usage of specific functions per file, or per
file with a matching (sha256) hash, thus allowing the use of such functions **only** in the intended places.
It can also restrict per `CIDR <https://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing>`__,
to restrict execution to users on the LAN for example. There are a *lot*
of different filters, so make sure to read the :ref:`corresponding documentation <virtual-patching-config>`.

Furthermore, running the `following script <https://github.com/jvoisin/snuffleupagus/blob/master/scripts/generate_rules.php>`_  will generate an hash and line-based whitelist
of dangerous functions, dropping them everywhere else:


.. literalinclude:: ../../scripts/generate_rules.php
   :language: php


The intent is to make post-exploitation process (such as backdooring of legitimate code, or RAT usage) a lot harder for the attacker.


.. _global-strict-feature:

Global strict mode
^^^^^^^^^^^^^^^^^^

By default, PHP will coerce values of the wrong type into the expected one
if possible. For example, if a function expecting an integer is given a string,
it will be coerced in an integer.

PHP7 introduced a **strict mode**, in which variables won't be coerced anymore,
and a `TypeError <https://php.net/manual/en/class.typeerror.php>`_ exception will
be raised if the types aren't matching.
`Scalar type declarations <https://secure.php.net/manual/en/migration70.new-features.php#migration70.new-features.scalar-type-declarations>`_
are optional, but you don't have to use them in your code to benefit from them,
since every internal function from php has them.

This option provides a switch to globally activate this strict mode,
helping to uncover vulnerabilities like the classical
`strcmp bypass <https://danuxx.blogspot.fr/2013/03/unauthorized-access-bypassing-php-strcmp.html>`_
and various other types mismatch.

This feature is largely inspired from the
`autostrict <https://github.com/krakjoe/autostrict>`_ module from `krakjoe <http://krakjoe.ninja>`__.

PHP8 already has `this feature <https://wiki.php.net/rfc/consistent_type_errors>`__ for internal functions.


.. _sloppy-comparisons-feature:

Preventing sloppy comparisons
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The aforementioned :ref:`strict mode <global-strict-feature>` only works with
annotated types and native functions, so it doesn't cover every instances of
`type juggling <https://secure.php.net/manual/en/language.types.type-juggling.php>`__
during comparisons. Since comparison between different types in PHP is
`notoriously <https://secure.php.net/manual/en/types.comparisons.php>`__
difficult to get right, Snuffleupagus offers a way to **always** use the
``identical`` operator instead of the ``equal`` one (see the `operator section <https://secure.php.net/manual/en/language.operators.comparison.php>`__
for PHP's documentation for more details), so that values with different types
will always be treated as being different.

Keep in mind that this feature will not only affect the ``==`` operator,
but also the `in_array
<https://secure.php.net/manual/en/function.in-array.php>`__, `array_search
<https://secure.php.net/manual/en/function.array-search.php>`__ and `array_keys
<https://secure.php.net/manual/en/function.array-keys.php>`__ functions.

PHP8 is implementing `a subset <https://wiki.php.net/rfc/consistent_type_errors>`__ of this feature.


.. _readonly-exec-feature:

Preventing execution of writable PHP files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If an attacker manages to upload an arbitrary file or to modify an existing one,
odds are that (thanks to the default `umask <https://en.wikipedia.org/wiki/Umask>`_)
this file is writable by the PHP process.

Snuffleupagus can prevent the execution of this kind of file. A good practice
would be to use a different user to run PHP than for administrating the website,
and using this feature to lock this up.

.. _stream-wrapper-whitelist-feature:

Whitelist of stream-wrappers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

PHP comes with a `lot of different <https://secure.php.net/manual/en/wrappers.php>`__
`stream wrapper <https://secure.php.net/manual/en/intro.stream.php>`__, and most of them
are enabled by default.

The only way to tighten a bit this exposition surface is to use the
`allow_url_fopen/allow_url_include <https://secure.php.net/manual/en/features.remote-files.php>`__
configuration options, but it's `not possible <https://bugs.php.net/bug.php?id=50715>`__
to deactivate them on an individual basis.

Examples of related vulnerabilities
"""""""""""""""""""""""""""""""""""

- `RCE via phar:// <https://github.com/orangetw/My-CTF-Web-Challenges#babyh-master-php-2017>`__
- `Data exfiltration via stream wrapper <https://www.idontplaydarts.com/2011/02/using-php-filter-for-local-file-inclusion/>`__
- `Inclusion via zip/phar <https://lightless.me/archives/include-file-from-zip-or-phar.html>`__

.. _php-stream-wrapper-allowlist-feature:

Allowlist of php stream-wrapper
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The builtin `"php" stream wrapper <https://www.php.net/manual/en/wrappers.php.php>`__
has support for common streams, like ``stdin``, ``stdout`` or ``stderr``, but
also for the dangerous ``filter`` one.

Examples of related vulnerabilities
"""""""""""""""""""""""""""""""""""

- `CNEXT exploits <https://github.com/ambionics/cnext-exploits/>`__
- Synacktiv's `php_filter_chain_generator <https://github.com/synacktiv/php_filter_chain_generator>`__ tool
- Ambionic's `wrapwrap <https://github.com/ambionics/wrapwrap>`__ tool

.. _eval-feature:

White and blacklist in ``eval``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

While `eval <https://secure.php.net/manual/en/function.eval.php>`__ is a
dangerous primitive, tricky to use right, with almost no legitimate usage
besides templating and building mathematical expressions based on user input,
it's broadly (mis)used all around the web.

Snuffleupagus provides a white and blacklist mechanism, to explicitly allow
and forbid specific function calls from being issued inside ``eval``.

While it's heavily recommended to only use the whitelist feature, the blacklist
one exists because some sysadmins might want to use it to catch automated
script-kiddies attacks, while being confident that doing so won't break a
single website.

.. _samesite-feature:

Protection against cross site request forgery
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cross-site request forgery, sometimes abbreviated as *CSRF*,
is when unauthorised commands are issued from a user that the application trusts.
For example, if a user is authenticated on a banking website,
an other site might present something like 
``<img src="http://mybank.com/transfer?from=user&to=attack&amount=1337EUR">``,
effectively transferring money from the user's account to the attacker one.

Snuffleupagus can prevent this (in `supported browsers <https://caniuse.com/#search=samesite>`__)
by setting the `samesite <https://tools.ietf.org/html/draft-ietf-httpbis-rfc6265bis-02#section-5.3.7>`__
attribute on cookies.


Dumping capabilities
^^^^^^^^^^^^^^^^^^^^
It's possible to apply the ``dump()`` filter to any virtual-patching rule,
to dump the complete web request, along with the filename and the corresponding
line number. By using the *right* set of restrictive rules (or by using the
*overly* restrictives ones in ``simulation`` mode), you might be able
to gather interesting vulnerabilities used against your website.

Dumps are stored in the folder that you pass to the ``dump()`` filter,
in files named ``sp_dump.SHA`` with ``SHA`` being the *sha256* of the
rule that matched. This approach allows to mitigate denial of services attacks
that could fill up your filesystem.


Misc low-hanging fruits in the default configuration file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Snuffleupagus is shipping with a default configuration file, containing
various examples and ideas of things that you might want to enable (or not).

Available functions recon
"""""""""""""""""""""""""

Usually after compromising a website the attacker does some recon
within its webshell, to check which functions are available to execute arbitrary code.
Since it's not uncommon for some web-hosts to disable things like ``system`` or ``passthru``,
or to check if mitigations are enabled, like ``open_basedir``.
This behaviour can be detected by preventing the execution of functions like ``ini_get``
or ``is_callable`` with *suspicious* parameters.

``chmod`` hardening
"""""""""""""""""""

Some PHP applications are using broad rights when using the ``chmod`` function,
like the infamous ``chmod(777)`` command, effectively making the file writable by everyone.
Snuffleupagus is preventing this kind of behaviour by restricting the parameters
that can be passed to ``chmod``.

Arbitrary file inclusion hardening
""""""""""""""""""""""""""""""""""

Arbitrary file inclusion is a common vulnerability, that might be detected
by preventing the inclusion of anything that doesn't match a strict set
of file extensions in calls to ``include`` or ``require``.


Enforcing certificate validation when using curl
""""""""""""""""""""""""""""""""""""""""""""""""

While it might be convenient to disable certificate validation on preproduction
or during tests, it's `common <https://twitter.com/CiPHPerCoder/status/1056974646363516928>`__
to see that people are disabling it on production too.
We're detecting/preventing this by not allowing the ``CURLOPT_SSL_VERIFYPEER`` and
``CURLOPT_SSL_VERIFYHOST`` options from being set to ``0``.

*Cheap* error-based SQL injections detection
""""""""""""""""""""""""""""""""""""""""""""

If a function performing a SQL query returns ``FALSE``
(indicating an error), it might be useful to dump the request for further analysis.

