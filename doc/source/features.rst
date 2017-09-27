Features
========

Snuffleupagus has a lot of features that can be divided in two main categories: bug-classes
killers and virtual-patching. The first category provides primitives to kill various
bug families (like arbitrary code execution via ``unserialize`` for example) or rise the 
cost of exploitation, the second one is a highly configurable system to patch functions in php itself.

Bug classes killed
------------------

``system`` injections
^^^^^^^^^^^^^^^^^^^^^

The ``system`` function execute an external program and displays the output.
It's used to interract with various external tools, like file-format converters for example.
Unfortunately, passing user-controlled parameters to it often leads to an arbitrary command execution.

  When allowing user-supplied data to be passed to this function,
  use `escapeshellarg()` or `escapeshellcmd()` to ensure that users cannot trick
  the system into executing arbitrary commands.

  --- `The PHP documentation about system <https://secure.php.net/manual/en/function.system.php>`_

We're kind of killing it by filtering the ``$``, ``|``, ``;``, ````` and ``&`` chars in our 
default configuration, making it a lot harder for an attacker to inject arbitrary commands.

This family of vulnerabilities lead to various CVE, like:

- `CVE-2017-7981 <https://tuleap.net/plugins/tracker/?aid=10159>`_: Authenticated remote code execution on Tuleap
- `CVE-2014-4688 <https://www.pfsense.org/security/advisories/pfSense-SA-14_10.webgui.asc>`_: Authenticated remote code execution on pfSense
- `CVE-2014-1610 <https://www.rapid7.com/db/modules/exploit/multi/http/mediawiki_thumb>`_: Unauthenticated remote code execution on DokuWiki
- `CVE-2013-3630 <https://www.rapid7.com/db/modules/exploit/multi/http/moodle_cmd_exec>`_: Authenticated remote code execution on Moodle
- Every single shitty `modem/router/switch/IoT <https://twitter.com/internetofshit>`_.


``mail``-related injections
^^^^^^^^^^^^^^^^^^^^^^^^^^^

This vulnerability is known `since 2011 <http://esec-pentest.sogeti.com/posts/2011/11/03/using-mail-for-remote-code-execution.html>`_,
and was popularized by `RIPS <https://www.ripstech.com/blog/2016/roundcube-command-execution-via-email/>`_ in 2016.
The last flag of the `mail` function can be used to pass various parameters to
the underlying binary used to send emails: this can lead to an arbitrary file write,
often meaning an arbitrary code execution.

  The ``additional_parameters`` parameter can be used to pass additional flags
  as command line options to the program configured to be used when sending mail

  --- `The PHP documentation about mail <https://secure.php.net/manual/en/function.mail.php>`_

We're killing it by preventing any extra options in additional_parameters.

This family of vulnerabilities lead to various CVE, like:

- `CVE-2017-7692 <https://legalhackers.com/advisories/SquirrelMail-Exploit-Remote-Code-Exec-CVE-2017-7692-Vuln.html>`_: Authenticated remote code execution in SquirrelMail
- `CVE-2016-10074 <https://legalhackers.com/advisories/SwiftMailer-Exploit-Remote-Code-Exec-CVE-2016-10074-Vuln.html>`_: remote code execution in SwiftMailer
- `CVE-2016-10033 <https://legalhackers.com/advisories/PHPMailer-Exploit-Remote-Code-Exec-CVE-2016-10033-Vuln.html>`_: remote code execution in PHPMailer
- `CVE-2016-9920 <https://www.ripstech.com/blog/2016/roundcube-command-execution-via-email/>`_: Unauthenticated remote code execution in Roundcube

Session-cookie stealing via XSS
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The goto payload for XSS is often to steal cookies.
Like *Suhosin*, we are encrypting the cookies with a secret key, the IP of the user
and its user-agent. This means that an attacker with an XSS won't be able to use
the stolen cookie, since he (often) can't spoof the IP address of the user.

This feature is roughly the same than the `Suhosin one <https://suhosin.org/stories/configuration.html#transparent-encryption-options>`_.

Users behind the same IP address but with different browsers won't be able to use each other stolen cookies,
except if they can manage to guess the user agent. This isn't especially difficult,
but an invalid decryption will leave a trace in the logs.

Finally, having a secret server-side key will prevent anyone (even the user himself)
from reading the content of the cookie, reducing the impact of an application storing sensitive data client-side.

The encryption is done via the `tweetnacl library <https://tweetnacl.cr.yp.to/>`_,
thus using curve25519, xsalsa20 and poly1305 for the encryption. We chose this
library because of its portability, simplicity and reduced size (a single `.h` and
`.c` file.).

Remote code execution via file-upload
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some PHP applications allows users to upload contents, like avatars for a forum.
Unfortunately, sometimes, content validation isn't implemented properly (if at all),
meaning arbitrary file upload, often leading, contrary to what the documentation is saying,
to an arbitrary code execution.

  Not validating which file you operate on may mean that users can *access sensitive information* in other directories.

  --- `The PHP documentation about file uploads <https://secure.php.net/manual/en/features.file-upload.common-pitfalls.php>`_

We're killing it, like Suhosin, by automatically calling a script upon file upload,
if it returns something else than ``0``, the file will be removed (or stored in a quarantine,
for further analysis).

We're recommending to use the `vld <https://derickrethans.nl/projects.html#vld>`_ project
inside the script to ensure the file doesn't contain any valid PHP code, with something like this:

::

  $ php -d vld.execute=0 -d vld.active=1 -d extension=vld.so $file

Unserialize-related magic
^^^^^^^^^^^^^^^^^^^^^^^^^

PHP is able to *serialize* arbitrary objects, to easily store them.
Unfortunately, it's often possible to gain arbitrary code execution upon deserialization
of user-supplied serialized objects.

  Do not pass untrusted user input to ``unserialize()`` regardless of the options value of allowed_classes.
  Unserialization can result in code being loaded and executed due to object instantiation and autoloading,
  and a malicious user may be able to exploit this.

  --- `The PHP documentation about serialize <https://secure.php.net/manual/en/function.serialize.php>`_

We're killing it by exploiting the fact that PHP will discard any garbage found at the end of a serialized object,
allowing us to simply append a `HMAC <https://en.wikipedia.org/wiki/Hash-based_message_authentication_code>`_
at the end of strings generated by the ``serialize``,
hence guaranteeing that any object deserialized came from the application,
and wasn't tampered with,

We're not encrypting it, like we do with the cookies,
allowing this feature to be disabled (or switch into leaning mode)
without the need to invalidate any data.

.. warning::

    This feature can't be deployed on websites that already stored serialized
    objects (ie. in database), since they are missing the HMAC, and thus will be detected as
    an attack. If you're in this situation, you should use this feature with the
    ``simulation`` mode, and switch it off once you don't have any messages in your
    logs.

A nice side-effect of this feature is that it'll defeat various memory corruption
issues related to the complexity of ``unserialize``'s implementation,
and the amount of control if provides to an attacker, like `CVE-2016-9137, CVE-2016-9138 <https://bugs.php.net/bug.php?id=73147>`_,
`2016-7124 <https://bugs.php.net/bug.php?id=72663>`_, `CVE-2016-5771 and CVE-2016-5773 <https://www.evonide.com/how-we-broke-php-hacked-pornhub-and-earned-20000-dollar/>`_, …

This family of vulnerabilities lead to various CVE, like:

- `CVE-2016-???? <https://www.computest.nl/advisories/CT-2016-1110_Observium.txt>`_: Unauthenticated remote code execution in Observium (leading to remote root)
- `CVE-2016-5726 <http://seclists.org/oss-sec/2016/q2/521>`_: Unauthenticated remote code execution in Simple Machines Forums
- `CVE-2016-4010 <http://netanelrub.in/2016/05/17/magento-unauthenticated-remote-code-execution/>`_: Unauthenticated remote code execution in Magento
- `CVE-2017-2641 <http://netanelrub.in/2017/03/20/moodle-remote-code-execution/>`_: Unauthenticated remote code execution in Moodle
- `CVE-2015-8562 <https://www.rapid7.com/db/modules/exploit/multi/http/joomla_http_header_rce>`_: Unauthenticated remote code execution in Joomla
- `CVE-2015-7808 <https://www.rapid7.com/db/modules/exploit/multi/http/vbulletin_unserialize>`_: Unauthenticated remote code execution in vBulletin
- `CVE-2014-1691 <http://seclists.org/oss-sec/2014/q1/153>`_: Unauthenticated remote code execution in Horde
- `CVE-2012-5692 <https://www.rapid7.com/db/modules/exploit/unix/webapp/invision_pboard_unserialize_exec>`_: Unauthenticated remote code execution in IP.Board


.. _harden-rand-feature:

Weak-PRNG via rand/mt_rand
^^^^^^^^^^^^^^^^^^^^^^^^^^

The functions ``rand`` and ``mt_rand`` are often used to generate random numbers used
in sensitive context, like password generation, token creation, …
Unfortunately, as said in the documentation, the quality of their entropy is low,
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

.. warning::

  Activating this feature will raise an `Error <https://secure.php.net/manual/en/class.error.php>`_
  exception if ``min`` is superior to ``max``, while the default dehaviour is simply to swap them.

This family of vulnerabilities lead to various CVE, like:

- `CVE-2015-5267 <https://moodle.org/mod/forum/discuss.php?d=320291>`_: Unauthenticated accounts takeover in in Moodle
- `CVE-2014-9624 <https://www.mantisbt.org/bugs/view.php?id=17984>`_: Captcha bypass in MantisBT
- `CVE-2014-6412 <https://core.trac.wordpress.org/ticket/28633>`_: Unauthenticated account takeover in Wordpress
- `CVE-2015-???? <https://hackerone.com/reports/31171>`_: Unauthenticated accounts takeover in Concrete5
- `CVE-2013-6386 <https://www.drupal.org/SA-CORE-2013-003>`_: Unauthenticated accounts takeover in Drupal
- `CVE-2010-???? <http://www.sektioneins.com/advisories/advisory-022010-mybb-password-reset-weak-random-numbers-vulnerability.html>`_: Unauthenticated accounts takeover in MyBB
- `CVE-2008-4102 <https://sektioneins.de/en/advisories/advisory-042008-joomla-weak-random-password-reset-token-vulnerability.html>`_: Unauthenticated accounts takeover in Joomla
- `CVE-2006-0632 <https://www.cvedetails.com/cve/CVE-2006-0632/>`_: Unauthenticated account takeover in phpBB

XXE
^^^

Despite the documentation saying nothing about this class of vulnerabilities,
`XML eXternal Entitiy <https://www.owasp.org/index.php/XML_External_Entity_(XXE)_Processing>`_  (XXE) are often leading to arbitrary file reading, SSRF, and sometimes even arbitrary
code execution.

XML documents can contain a `Document Type Definition <https://www.w3.org/TR/REC-xml/#sec-prolog-dtd>`_ (DTD),
enabling definition of XML entities. It's possible to define an (external) entity by an
URI, that the parser will access, and embed its content back into the document
for further processing.

For example, providing an url like ``file:///etc/passwd`` will read
this file's content, and since it's not valid XML, the application
will spit it out in an error message, thus leaking its content.

We're killing this class of vulnerabilities by calling
the `libxml_disable_entity_loader <https://secure.php.net/manual/en/function.libxml-disable-entity-loader.php>`_
function with its parameter set to ``true`` at startup,
and then *nop'ing* it, so it won't do anything if ever called again.

This family of vulnerabilities lead to various CVE, like:

- `CVE-2015-5161 <https://legalhackers.com/advisories/eBay-Magento-XXE-Injection-Vulnerability.html>`_: Unauthenticated arbitrary file disclosure on Magento
- `CVE-2014-8790 <https://github.com/GetSimpleCMS/GetSimpleCMS/issues/944>`_: Unauthenticated remote code execution in GetSimple CMS
- `CVE-2011-4107 <https://www.phpmyadmin.net/security/PMASA-2011-17/>`_: Authenticated local file disclosure in PHPMyAdmin


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
``system()``, ``exec()``, ``dlopen()`` - for example - fall into this category. By default, PHP only allows to globally disable some functions.


However, (ie. ``system()``) they might have legitimate use cases in processes such as self upgrade etc., making it impossible to effectively
disable them - at the risk of breaking critical features.

SnuffleuPagus allows the user to restrict usage of specific functions per files, or per
files with a matching (sha256) hash, thus allowing the use of such functions **only** in the intended places.

Furthermore, running the `following script <FIXME>`_  will generate an hash and line-based whitelist
of dangerous functions, droping them everywhere else:


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

This option provide a switch to globally activate this strict mode,
helping to uncover vulnerabilities like the classical
`strcmp bypass <https://danuxx.blogspot.fr/2013/03/unauthorized-access-bypassing-php-strcmp.html>`_,
and various other types mismatch.

This feature is largely inspired from the
`autostrict <https://github.com/krakjoe/autostrict>`_ module from `krakjoe <krakjoe.ninja>`_.


Preventing execution of writable PHP files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If an attacker manages to upload an arbitrary file or to modify an existing one,
odds are that (thanks to the default `umask <https://en.wikipedia.org/wiki/Umask>`_)
this file is writable by the PHP process.

Snuffleupagus can prevent the execution of this kind of files. A good practise
would be to use a different user to run PHP than for administrating the website,
and using this feature to lock this up.



Dumping capabilities
^^^^^^^^^^^^^^^^^^^^
It's possible to apply the ``dump()`` filter to any virtual-patching rule,
to dump the complete web request, along with the filename and the corresponding 
line number. By using the *right* set of restrictive rules (or by using the
*overly* restrictives ones in ``simulation`` mode), you might be able
to gather interesting vulnerabilities used against your website.


Misc low-hanging fruits in the default configuration file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Snuffleupagus is shipping with a default configuration file, containing
various examples and ideas of things that you might want to enable (or not).

Available functions recon
"""""""""""""""""""""""""

After compromising a website, most of the time, the attacker does some recon
within its webshell, to check which functions are available to execute arbitrary code,
since it's not uncommon for some web-hoster to disable things like ``system`` or ``passthru``,
or to check if mitigations are enabled, like ``open_basedir``.
This behaviour can be detected by preventing the execution of functions like ``ini_get``
or ``is_callable`` with *suspicious* parameters.

``chmod`` hardening
"""""""""""""""""""

Some PHP applications are using broad rights when using the ``chmod`` function, 
like the infamous ``chmod(777)`` command, effectively making the file writable by everyone.
Snuffleupagus is preventing this kind of behaviour by restricting the parameters
than can be passer to ``chmod``.

Arbitrary file inclusion hardening
""""""""""""""""""""""""""""""""""

Arbitrary file inclusion is a common vulnerability, that might be detected
by preventing the inclusion of anything that doens't match a strict set
of file extensions in calls to ``include`` or ``require``.

*Cheap* SQL injections detection
""""""""""""""""""""""""""""""""

In some SQL injections, attackers might need to use comments, a feature that is
often not used in production system, so it might be a good idea to filter
queries that contains some. The same filtering idea can be used against
SQL functions that are frequently used in SQL injections, like ``sleep``, ``benchmark``
or strings like ``version_info``.

Still about SQL injections, if a function performing a query returns ``FALSE``
(indicating an error), it might be useful to dump the request for further analysis.

