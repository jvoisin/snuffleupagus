Configuration
=============

.. warning::

  If you configure Snuffleupagus incorrectly, your website *might* not work
  correctly until either you fix your configuration, or revert your changes
  altogether.

  It's up to you to understand the :doc:`features <features>`,
  read the present documentation about how to configure them,
  evaluate your threat model and write your configuration file accordingly.

Since PHP *ini-like* configuration model isn't flexible enough,
Snuffleupagus is using its own format in the file specified by
the directive ``sp.configuration_file`` **in** your ``php.ini`` file,
like ``sp.configuration_file=/etc/php/conf.d/snuffleupagus.rules``.

You can use the ``,`` separator to include multiple configuration files:
``sp.configuration_file=/etc/php/conf.d/snuffleupagus.rules,/etc/php/conf.d/sp_wordpress.rules``.

We're also also supporting `glob <https://en.wikipedia.org/wiki/Glob_%28programming%29>`__,
so you can write something like:
``sp.configuration_file=/etc/php/conf.d/*.rules,/etc/php/conf.d/extra/test.rules``.

To sum up, you should put this in your ``php.ini``:

::

  module=snuffleupagus.so
  sp.configuration_file=/path/to/your/snuffleupagus/rules/file.rules

And the **snuffleupagus rules** into the ``.rules`` files.

Since our configuration format is a bit more complex than php's one,
we have a ``sp.allow_broken_configuration`` parameter (``false`` by default),
that you can set to ``true`` if you want PHP to carry on if your Snuffleupagus'
configuration contains syntax errors. You'll still get a big scary message in
your logs of course. We do **not** recommend to use it of course, but sometimes
it might be useful to be able to "debug in production" without breaking your
website.

Configuration file format
-------------------------

Options are chainable by using dots (``.``).

Some options have a string parameter, that **must** be quoted with double quotes, e.g. ``"string"``.

Comments are prefixed either with ``#``, or ``;``.

Some rules apply in a specific ``function`` (context) on a specific ``variable``
(data), like ``disable_function``. Others can only be enabled/disabled, like
``harden_random``.

Most of the features can be used in ``simulation`` mode by appending the
``.simulation()`` or ``.sim()`` option to them (eg. ``sp.readonly_exec.simulation().enable();``) to see
whether or not they could break your website. The simulation mode won't block the request,
but will write a warning in the log.

The rules are evaluated in the order that they are written, the **first** one
to match will terminate the evaluation (except for rules in simulation mode).

Rules can be split into lines and contain whitespace for easier readability and maintenance: (This feature is available since version 0.8.0.)

::

  sp.disable_function.function("mail")
    .param("to").value_r("\\n")
    .alias("newline in mail() To:")
    .drop();

The terminating ``;`` is optional for now, but it should be used for future compatibility.

Rules, including comments, needs to be written in ASCII, other encodings aren't
supported and might cause syntax errors and related issues like making
all rules after non-ASCII symbols not considered for execution and silently discarded.

Miscellaneous
-------------

conditions
^^^^^^^^^^

It's possible to use conditions to have configuration portable across
several setups.

::

  @condition PHP_VERSION_ID < 80000;
    # some rules
  @condition PHP_VERSION_ID >= 80000;
    # some other rules
  @end_condition;

Conditions accept variables and the special function ``extension_loadod()``.

::

  @condition extension_loaded("sqlite3");
  sp.ini.key("sqlite3.extension_dir").ro();
  @end_condition;

Conditions cannot be nested, but arithmetic and logical operations can be applied.

::

  @condition extension_loaded("session") && PHP_VERSION_ID <= 80200;
  set whitelist "my_fun,cos"
  sp.eval_whitelist.list(whitelist).simulation().dump("/tmp/dump_result/");
  @end_condition;

variables
^^^^^^^^^

You may set a configuration variable using the ``set`` keyword (or ``@set``) and use it instead of arguments.

::

  @set CMD "ls"
  sp.disable_function.function("system").pos("0").value(CMD).allow();

global
^^^^^^

This configuration variable contains parameters that are used by multiple features:

- ``secret_key``: A secret key used by various cryptographic features,
  like `cookies protection <features.html#session-cookie-stealing-via-xss>`__ or `unserialize protection <features.html#unserialize-related-magic>`__,
  please ensure the length and complexity is sufficient.
  You can generate it with functions such as: ``head -c 256 /dev/urandom | tr -dc 'a-zA-Z0-9'``.

::

  sp.global.secret_key("44239bd400aa82e125337c9d4eb8315767411ccd");

- ``cookie_env_var``: A environment variable used as part of cookies encryption.
  See the :ref:`relevant documentation <config_cookie-encryption>`

log_media
^^^^^^^^^

This configuration variable allows to specify how logs should be written,
either via ``php`` or ``syslog``.

::

  sp.log_media("php");
  sp.log_media("syslog");

The default value for ``sp.log_media`` is ``php``, to respect the `principle of
least astonishment
<https://en.wikipedia.org/wiki/Principle_of_least_astonishment>`__. But since
it's `possible to modify php's logging system via php
<https://www.php.net/manual/en/errorfunc.configuration.php>`__, it's
heavily recommended to use the ``syslog`` option instead.

log_max_len
^^^^^^^^^^^

This configuration variable allows to specify (roughly) the size of the log.

::

  sp.log_max_len("16");

The default value for ``sp.log_max_len`` is ``255``.

Bugclass-killer features
------------------------

global_strict
^^^^^^^^^^^^^

:ref:`global_strict <global-strict-feature>`, disabled by default, will enable the `strict <https://secure.php.net/manual/en/functions.arguments.php#functions.arguments.type-declaration.strict>`_ mode globally, 
forcing PHP to throw a `TypeError <https://secure.php.net/manual/en/class.typeerror.php>`_
exception if an argument type being passed to a function does not match its corresponding declared parameter type.

It can either be ``enabled`` or ``disabled``.

::

  sp.global_strict.disable();
  sp.global_strict.enable();

harden_random
^^^^^^^^^^^^^
 
:ref:`harden_random <harden-rand-feature>`, enabled by default, will silently
replace the insecure `rand
<https://secure.php.net/manual/en/function.rand.php>`_ and `mt_rand
<https://secure.php.net/manual/en/function.mt-rand.php>`_ functions with the
secure PRNG `random_int
<https://secure.php.net/manual/en/function.random-int.php>`_.

It can either be ``enabled`` or ``disabled``.

::

  sp.harden_random.enable();
  sp.harden_random.disable();

.. _config_global:

Prevent sloppy comparison
^^^^^^^^^^^^^^^^^^^^^^^^^

:ref:`Sloppy comparison prevention <sloppy-comparisons-feature>`, disabled by default, will prevent php `type
juggling <https://secure.php.net/manual/en/language.types.type-juggling.php>`_ (``==``):
two values with different types will always be different.

It can either be ``enabled`` or ``disabled``.

::

  sp.sloppy_comparison.enable();
  sp.sloppy_comparison.disable();

unserialize_noclass
^^^^^^^^^^^^^^^^^^^
 
:ref:`unserialize_noclass <unserialize-feature>`, available only on PHP8+ and
disabled by default, will disable the deserialization of objects via
``unserialize``. It's equivalent to setting the ``options`` parameter of
``unserialize`` to ``false``, on every call. It can either be ``enabled`` or
``disabled``.

::

  sp.unserialize_noclass.enable();
  sp.unserialize_noclass.disable();

unserialize_hmac
^^^^^^^^^^^^^^^^
 
:ref:`unserialize_hmac <unserialize-feature>`, disabled by default, will add an
integrity check to ``unserialize`` calls, preventing arbitrary code execution
in their context.

It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.

::

  sp.unserialize_hmac.enable();
  sp.unserialize_hmac.disable();


.. warning::

  This feature breaks web applications doing checks on the serialized
  representation of data on their own, like `WordPress <https://wordpress.com/>`__.

.. _config_cookie-encryption:

Cookies-related mitigations
^^^^^^^^^^^^^^^^^^^^^^^^^^^
 
Since snuffleupagus is providing several hardening features for cookies,
there is a dedicated web page :ref:`here <cookie-encryption-page>` about them.

INI Settings Protection
^^^^^^^^^^^^^^^^^^^^^^^
INI settings can be forced to a value, limited by min/max value or regular expression and set read-only mode.

First, this feature can be enabled or disabled:

::

  sp.ini_protection.enable();
  sp.ini_protection.disable();

The INI protection feature can be set to simulation mode, where violations are only reported, but rules are not enforced:

::

  sp.ini_protection.simulation();

Rule violations can be set to drop as a global policy, or alternatively be set on individual rules using ``.drop()``.

::

  sp.ini_protection.policy_drop();

Rules can be set to fail silently without logging anything:

::

  sp.ini_protection.policy_silent_fail();
  ## or write sp.ini_protection.policy_no_log(); as an alias

Read-only settings are implemented in a way that the PHP system itself can block the setting, which is very efficient. If you do not need to log read-only violations, these can be set to silent separately:

::

  sp.ini_protection.policy_silent_ro();

A global access policy can be set to either read-only or read-write. Individual entries can be set to read-only/read-write as well using ``.ro()``/``.rw()``.

::

  sp.ini_protection.policy_readonly();
  sp.ini_protection.policy_readwrite();

Individual rules are specified using ``sp.ini``. These entries can have the following attributes:

- ``.key("...")``: mandatory ini name.
- ``.set("...")``: set the initial value. This overrides php.ini. checks are not performed for this initial value.
- ``.min("...")`` / ``.max("...")``: value must be an integer between .min and .max. shorthand notation (e.g. 1k = 1024) is allowed
- ``.regexp("...")``: value must match the regular expression
- ``.allow_null()``: allow setting a NULL-value
- ``.msg("...")``: message is shown in logs on rule violation instead of default message
- ``.readonly()`` / ``.ro()`` / .readwrite() / .rw(): set entry to read-only or read-write respectively. If no access keyword is provided, the entry inherits the default policy set by ``sp.ini_protection.policy_*``-rules.
- ``.drop()``: drop request on rule violation for this entry
- ``.simulation()``: only log rule violation for this entry

Examples:

::

  sp.ini.key("display_errors").set("0").ro();
  sp.ini.key("default_socket_timeout").min("1").max("300").rw();
  sp.ini.key("highlight.comment").regexp("^#[0-9a-fA-F]{6}$");

For more examples, check out the ``config`` directory.

readonly_exec
^^^^^^^^^^^^^

:ref:`readonly_exec <readonly-exec-feature>`, disabled by default, will prevent
the execution of writeable PHP files.

It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.
``extended_checks`` can be specified to abort the execution if the executed
file or the folder containing it is owned by the user the PHP process is
running under.

Extended checks, enabled by default, can be explicitly enabled via
``extended_checks`` and disabled via ``no_extended_checks``. The checks
include:

* verifying the effective user id;
* verifying that the current folder isn't writable;
* verifying the current folder effective user id.

::

  sp.readonly_exec.enable();

upload_validation
^^^^^^^^^^^^^^^^^

:ref:`upload_validation <fileupload-feature>`, disabled by default, will call a
given script upon a file upload, with the path to the file being uploaded as
argument and various information about it in the environment:

* ``SP_FILENAME``: the name of the uploaded file
* ``SP_FILESIZE``: the size of the file being uploaded
* ``SP_REMOTE_ADDR``: the ip address of the uploader
* ``SP_CURRENT_FILE``: the current file being executed

This feature can be used, for example, to check if an uploaded file contains php
code, using `vld <https://derickrethans.nl/projects.html#vld>`_,
via `a python script <https://github.com/jvoisin/snuffleupagus/tree/master/scripts/upload_validation.py>`__,
or `a php one <https://github.com/jvoisin/snuffleupagus/tree/master/scripts/upload_validation.php>`__.

The upload will be **allowed** if the script returns the value ``0``. Every other
value will prevent the file from being uploaded.

It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.

::

  sp.upload_validation.script("/var/www/is_valid_php.py").enable();


xxe_protection
^^^^^^^^^^^

:ref:`xxe_protection <xxe-feature>`, disabled by default, will prevent XXE attacks by disabling the loading of external entities (``libxml_disable_entity_loader``) in the XML parser.

::

  sp.xxe_protection.enable();
  sp.xxe_protection.disable();


Whitelist of stream-wrappers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:ref:`Stream-wrapper whitelist <stream-wrapper-whitelist-feature>` allows
to explicitly whitelist some `stream wrappers <https://secure.php.net/manual/en/intro.stream.php>`__.

::

  sp.wrappers_whitelist.list("file,php,phar");


Allowlist of the php stream-wrapper
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:ref:`The php-stream-wrapper allowlist <php-stream-wrapper-allowlist-feature>`
allows to explicitly restrict the permitted builtin `php streams <https://www.php.net/manual/en/wrappers.php.php>`__.

::

  sp.wrappers_whitelist.php_list("stdout,stdin,stderr");


Eval white and blacklist
^^^^^^^^^^^^^^^^^^^^^^^^

:ref:`eval_whitelist and eval_blacklist <eval-feature>`, disabled by default,
allow to respectively specify functions allowed and forbidden from being called
inside ``eval``. The functions names are comma-separated.

::

  sp.eval_blacklist.list("system,exec,shell_exec");
  sp.eval_whitelist.list("strlen,strcmp").simulation();

The whitelist comes before the black one: if a function is both whitelisted and
blacklisted, it'll be allowed.


.. _virtual-patching-config:

Virtual-patching
----------------

Snuffleupagus provides virtual-patching via the ``disable_function`` directive,
allowing you to stop or control dangerous behaviours.  In the situation where
you have a call to ``system()`` that lacks proper user-input validation, this
could cause issues as it would lead to an **RCE**. The virtual-patching would
allow this to be prevented.

::
   
  # Allow `id.php` to restrict system() calls to `id`
  sp.disable_function.function("system").filename("/var/www/html/id.php").param("cmd").value("id").allow();
  sp.disable_function.function("system").filename("/var/www/html/id.php").drop()

Of course, this is a trivial example,  a lot can be achieved with this feature, as you will see below.


Filters
^^^^^^^

- ``alias(description)``: human-readable ``description`` of the rule
- ``cidr(ip/mask)``: match on the client's `cidr <https://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing>`_
- ``filename(name)``: exact match on the file's ``name``
- ``filename_r(regexp)``: file name matching the ``regexp``
- ``function(name)``: exact match on function ``name``
- ``function_r(regexp)``: function name matching the ``regexp``
- ``hash(sha256)``: exact match on the file's `sha256 <https://en.wikipedia.org/wiki/SHA-2>`_ sum
- ``line(line_number)``: exact match on the file's line.
- ``param(name)``: exact match on the function's parameter ``name``
- ``param_r(regexp)``: match on the function's parameter ``regexp``
- ``param_type(type)``: exact match on the function's parameter ``type``
- ``pos(nth_argument)``: exact match on the nth argument, starting from ``0``
- ``ret(value)``: exact match on the function's return ``value``
- ``ret_r(regexp)``: match with a ``regexp`` on the function's return
- ``ret_type(type_name)``: match on the ``type_name`` of the function's return value
- ``value(value)``: exact match on a literal ``value``
- ``value_r(regexp)``: match on a value matching the ``regexp``
- ``var(name)``: exact match on a **local variable** ``name``
- ``key(name)``: exact match on the presence of ``name`` as a key in the hashtable
- ``key_r(regexp)``: match with ``regexp`` on keys in the hashtable

The ``type`` must be one of the following values:

- ``FALSE``: for boolean false
- ``TRUE``: for boolean true
- ``NULL``: for the **null** value
- ``LONG``: for a long (also know as ``integer``) value
- ``DOUBLE``: for a **double** (also known as ``float``) value
- ``STRING``: for a string
- ``OBJECT``: for a object
- ``ARRAY``: for an array
- ``RESOURCE``: for a resource

Actions
^^^^^^^

Every rule *must* have one action.

- ``allow()``: **allow** the request if the rule matches
- ``drop()``: **drop** the request if the rule matches

Modifications
^^^^^^^^^^^^^

- ``dump(directory)``: dump the request in the ``directory`` if it matches the rule
- ``simulation()``: enabled the simulation mode

Details
^^^^^^^

The ``function`` filter is able to do various dereferencing:

- ``function("AwesomeClass::my_method")`` will match the method ``my_method`` in the class ``AwesomeClass``
- ``function("AwesomeNamespace\\my_function")`` will match the function ``my_function`` in the namespace ``AwesomeNamespace``

It's also able to have calltrace constrains: ``function(func1>func2)`` will
match only if ``func2`` is called **inside** of ``func1``. Do note that their
might be other functions called between them.

The ``param`` filter is able to do some dereferencing as well:

- ``param($foo[bar])`` will get a match on the value corresponding to the ``bar`` key in the hashtable ``foo``.
  Remember that in PHP, almost every data structure is a hashtable. You can of course nest this like
  ``param($foo[bar][$object->array['123']][$batman])``.
- The ``var`` filter will walk the calltrace until it finds the variable name, or the end of the calltrace,
  allowing the filter to match global variables: ``.var("$_GET[\"param\"]")`` will match on the GET parameter ``param``.

The ``filename`` filter requires a leading ``/``, since paths are absolutes (like ``/var/www/mywebsite/lib/parse.php``).
If you would like to have only one configuration file for several vhost in different folders,
you can use the ``filename_r`` directive to match on the filename (like ``/lib/parse\.php``).
Please do note that this filter matches on the file where the function is **defined**,
not the one where the function is **called from**.

For clarity, the presence of the ``allow`` or ``drop`` action is **mandatory**.

In the logs, the parameters and the return values of function are url-encoded,
to accommodate fragile log processors.

.. warning::

  When you're writing rules, please do keep in mind that **the order matters**.
  For example, if you're denying a call to ``system()`` and then allowing it in a
  more narrowed way later, the call will be denied,
  because it'll match the deny first.

If you're paranoid, we're providing a `php script
<https://github.com/jvoisin/snuffleupagus/blob/master/scripts/generate_rules.php>`__
to automatically generate hash of files containing dangerous functions, and
blacklisting them everywhere else.

Limitations
^^^^^^^^^^^

It's currently not possible to:

- Hook every `language construct <https://secure.php.net/manual/en/reserved.keywords.php>`__,
  because each of them requires a specific implementation. It's also not
  possible to hook them via regular expression.
- Use extra-convoluted rules for matching, like ``${$A}$$B->${'}[1]``, because if you're writing
  things like this, odds are that you're doing something wrong anyway.
- Hooks on ``echo`` and on ``print`` are equivalent: there is no way to hook one
  without hooking the other, at least
  `for now <https://github.com/jvoisin/snuffleupagus/issues/190>`__).
  This is why hooked ``print`` will be displayed as ``echo`` in the logs.
- Hook `strlen`, since in latest PHP versions, this function is usually
  optimized away by the compiler.


Examples
^^^^^^^^

Evaluation order of rules
"""""""""""""""""""""""""

The following rules will:

1. Allow calls to ``system("id")``
2. Issue a trace in the logs on calls to ``system`` with its parameters starting with ``ping``,
   and pursuing evaluation of the remaining rules.
3. Drop calls to ``system``.


::

  sp.disable_function.function("system").param("cmd").value("id").allow();
  sp.disable_function.function("system").param("cmd").value_r("^ping").drop().simulation();
  sp.disable_function.function("system").param("cmd").drop();

Miscellaneous examples
""""""""""""""""""""""

.. literalinclude:: ../../config/default.rules
   :language: python
