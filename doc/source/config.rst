Configuration
=============

Since PHP *ini-like* configuration model isn't flexible enough,
Snuffleupagus is using its own format in the file specified by
the directive ``sp.configuration_file`` (in your ``php.ini`` file),
like ``sp.configuration_file=/etc/php/conf.d/snuffleupagus.ini``.

You can use the ``,`` separator to include multiple configuration files :
``sp.configuration_file=/etc/php/conf.d/snuffleupagus.ini,/etc/php/conf.d/sp_wordpress.ini``

Options are chainable by using dots (``.``) and string parameters
**must** be quoted, while booleans and integers aren't.

Comments are prefixed either with ``#``, or ``;``.

Some rules apply in a specific ``function`` (context) on a specific ``variable``
(data), like ``disable_function``. Others can only be enabled/disabled, like
``harden_random``.


.. warning::

  If you configure Snuffleupagus incorrectly, you could break your website.
  It's up to you to understand the :doc:`features <features>`,
  read the present documentation about how to configure them,
  evaluate your threat model and write your configuration file accordingly.

Most of the features can be used in ``simulation`` mode by appending the
``.simulation()`` option to them (eg. ``sp.readonly_exec.simulation().enable();``) to see
whether or not they could break your website. The simulation mode won't block the request,
but will write a warning in the log.

The rules are evaluated in the order that they are written, the **first** one
to match will terminate the evaluation (except for rules in simulation mode).

Bugclass-killer features
------------------------

global_strict
^^^^^^^^^^^^^
`default: disabled`

``global_strict`` will enable the `strict <https://secure.php.net/manual/en/functions.arguments.php#functions.arguments.type-declaration.strict>`_ mode globally, 
forcing PHP to throw a `TypeError <https://secure.php.net/manual/en/class.typeerror.php>`_
exception if an argument type being passed to a function does not match its corresponding declared parameter type.

It can either be ``enabled`` or ``disabled``.

::

  sp.global_strict.disable();
  sp.global_strict.enable();

harden_random
^^^^^^^^^^^^^
 * `default: enabled`
 * `more <features.html#weak-prng-via-rand-mt-rand>`__
 
``harden_random`` will silently replace the insecure `rand <https://secure.php.net/manual/en/function.rand.php>`_
and `mt_rand <https://secure.php.net/manual/en/function.mt-rand.php>`_ functions with
the secure PRNG `random_int <https://secure.php.net/manual/en/function.random-int.php>`_.

It can either be ``enabled`` or ``disabled``.

::

  sp.harden_random.enable();
  sp.harden_random.disable();

.. _config_global:

global
^^^^^^

This configuration variable contains parameters that are used by multiple functions:

- ``secret_key``: A secret key used by various cryptographic features,
  like `cookies protection <features.html#session-cookie-stealing-via-xss>`__ or `unserialize protection <features.html#unserialize-related-magic>`__,
  please ensure the length and complexity is sufficient.
  You can generate it with functions such as: ``head -c 256 /dev/urandom | tr -dc 'a-zA-Z0-9'``.

::

  sp.global.secret_key("44239bd400aa82e125337c9d4eb8315767411ccd");

unserialize_hmac
^^^^^^^^^^^^^^^^
 * `default: disabled`
 * `more <features.html#unserialize-related-magic>`__
 
``unserialize_hmac`` will add an integrity check to ``unserialize`` calls, preventing
abritrary code execution in their context.

It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.

::

  sp.unserialize_hmac.enable();
  sp.unserialize_hmac.disable();


auto_cookie_secure
^^^^^^^^^^^^^^^^^^
 * `default: disabled`
 * `more <features.html#session-cookie-stealing-via-xss>`__
 
``auto_cookie_secure`` will automatically mark cookies as `secure <https://en.wikipedia.org/wiki/HTTP_cookie#Secure_cookie>`_
when the web page is requested over HTTPS.

It can either be ``enabled`` or ``disabled``.

::

  sp.auto_cookie_secure.enable();
  sp.auto_cookie_secure.disable();

cookie_samesite
^^^^^^^^^^^^^^^^
 * `default: disabled`
 
``samesite`` will add the `samesite <https://tools.ietf.org/html/draft-west-first-party-cookies-07>`_
attribute to cookies. It `prevents CSRF <https://www.owasp.org/index.php/SameSite>`_
but is not implemented by `all web browsers <https://caniuse.com/#search=samesite>`_ yet.

It can either be set to ``strict`` or ``lax``:

- The ``lax`` attribute prevents cookies from being sent cross-domain for
  "dangerous" methods, like ``POST``, ``PUT`` or ``DELETE``.

- The ``strict`` one prevents any cookies from beind sent cross-domain.

::

  sp.cookie.name("cookie1").samesite("lax");
  sp.cookie.name("cookie2").samesite("strict");;

.. _cookie-encryption_config:

cookie_encryption
^^^^^^^^^^^^^^^^^

 * `default: disabled`
 * `more <features.html#session-cookie-stealing-via-xss>`__
   
.. warning::

  To use this feature, you **must** set the :ref:`global.secret_key <config_global>`
  and the :ref:`global.cookie_env_var <config_global>` variables.
  This design decision prevents an attacker from
  `trivially bruteforcing <https://www.idontplaydarts.com/2011/11/decrypting-suhosin-sessions-and-cookies/>`_
  or re-using session cookies.

``cookie_secure`` will activate transparent encryption of specific cookies.

It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.

::

  sp.cookie.name("my_cookie_name").encrypt();
  sp.cookie.name("another_cookie_name").encrypt();


Removing the user-agent part
""""""""""""""""""""""""""""

Some web browser extensions, such as `uMatrix <https://github.com/gorhill/uMatrix/wiki>`__
might be configured to change the user-agent on a regular basis. If you think that
some of your users might be using configurations like this, you might want to disable
the mixing of the user-agent in the cookie's encryption key. The simplest way to do
so is to set the environment variable ``HTTP_USER_AGENT`` to a fixed value before passing
it to your php process.

We think that this use case is too exotic to be worth implementing as a
proper configuration directive.

Choosing the proper environment variable
""""""""""""""""""""""""""""""""""""""""

It's up to you to choose a meaningful environment variable to derive the key from.
Suhosin `is using <https://www.suhosin.org/stories/configuration.html#suhosin-session-cryptraddr>`_
the ``REMOTE_ADDR`` one, tying the validity of the cookie to the IP address of the user;
unfortunately, nowadays, people are `roaming <https://en.wikipedia.org/wiki/Roaming>`_ a lot on their smartphone,
hopping from WiFi to 4G.

This is why we recommend, if possible, to use the *extended master secret*
from TLS connections (`RFC7627 <https://tools.ietf.org/html/rfc7627>`_)
instead. The will make the validity of the cookie TLS-dependent, by using the ``SSL_SESSION_ID`` variable.

- In `Apache <https://httpd.apache.org/docs/current/mod/mod_ssl.html>`_,
  it is possible to enable by adding ``SSLOptions StdEnvVars`` in your Apache2 configuration.
- In `nginx <https://nginx.org/en/docs/http/ngx_http_ssl_module.html#variables>`_,
  you have to use ``fastcgi_param SSL_SESSION_ID $ssl_session_id if_not_empty;``.

If you aren't using TLS (you should be), you can always use the ``REMOTE_ADDR`` one,
or ``X-Real-IP`` if you're behind a reverse proxy.

readonly_exec
^^^^^^^^^^^^^
 * `default: disabled`

``readonly_exec`` will prevent the execution of writable PHP files.

It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.

::

  sp.readonly_exec.enable();

upload_validation
^^^^^^^^^^^^^^^^^
 * `default: disabled`
 * `more <features.html#remote-code-execution-via-file-upload>`__

``upload_validation`` will call a given script upon a file upload, with the path 
to the file being uploaded as argument and various information about it in the environment:

* ``SP_FILENAME``: the name of the uploaded file
* ``SP_FILESIZE``: the size of the file being uploaded
* ``SP_REMOTE_ADDR``: the ip address of the uploader
* ``SP_CURRENT_FILE``: the current file being executed

This feature can be used, for example, to check if an uploaded file contains php
code, with something like `vld <https://derickrethans.nl/projects.html#vld>`_
(``php -d vld.execute=0 -d vld.active=1 -d extension=vld.so yourfile.php``).

The upload will be **allowed** if the script returns the value ``0``. Every other
value will prevent the file from being uploaded.

It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.

::

  sp.upload_validation.script("/var/www/is_valid_php.py").enable();


disable_xxe
^^^^^^^^^^^
 * `default: enabled`
 * `more <features.html#xxe>`__

``disable_xxe`` will prevent XXE attacks by disabling the loading of external entities (``libxml_disable_entity_loader``) in the XML parser.

::

  sp.disable_xxe.enable();


Eval white and blacklist
^^^^^^^^^^^^^^^^^^^^^^^^
 * `default: disabled`
 * :ref:`more <eval-feature>`

``eval_blacklist`` allows to specify white and blacklist of functions allowed and
forbidden from being called inside ``eval``. The functions names are comma-separated.

::

  sp.eval_blacklist.list("system,exec,shell_exec");
  sp.eval_blacklist.list("strlen,strcmp").simulation();


Virtual-patching
----------------

Snuffleupagus provides virtual-patching via the ``disable_function`` directive, allowing you to stop or control dangerous behaviours.
In the situation where you have a call to ``system()`` that lacks proper user-input validation, this could cause issues as it would lead to an **RCE**. The virtual-patching would allow this to be prevented.

::
   
  # Allow `id.php` to restrict system() calls to `id`
  sp.disable_function.function("system").filename("id.php").param("$cmd").value("id").allow();
  sp.disable_function.function("system").filename("id.php").drop()

Of course, this is a trivial example,  a lot can be achieved with this feature, as you will see below.


Filters
^^^^^^^

- ``alias(description)``: human-readable ``description`` of the rule
- ``cidr(ip/mask)``: match on the client's `cidr <https://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing>`_
- ``filename(name)``: match in the file ``name``
- ``filename_r(regexp)``: the file name matching the ``regexp``
- ``function(name)``: match on function ``name``
- ``function_r(regexp)``: the function matching the ``regexp``
- ``hash(sha256)``: match on the file's `sha256 <https://en.wikipedia.org/wiki/SHA-2>`_ sum
- ``line(line_number)``: match on the file's line.
- ``param(name)``: match on the function's parameter ``name``
- ``param_r(regexp)``: match on the function's parameter ``regexp``
- ``param_type(type)``: match on the function's parameter ``type``
- ``ret(value)``: match on the function's return ``value``
- ``ret_r(regexp)``: match with a ``regexp`` on the function's return
- ``ret_type(type_name)``: match on the ``type_name`` of the function's return value
- ``value(value)``: match on a literal ``value``
- ``value_r(regexp)``: match on a value matching the ``regexp``
- ``var(name)``: match on a **local variable** ``name``
- ``key(name)``: match on the presence of ``name`` as a key in the hashtable
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

- ``allow()``: **allow** the request if the rule matches
- ``drop()``: **drop** the request if the rule matches
- ``dump(directory)``: dump the request in the ``directory`` if it matches the rule
- ``simulation()``: enabled the simulation mode

Details
^^^^^^^

The ``function`` filter is able to do various dereferencing:

- ``function("AwesomeClass::my_method")`` will match the method ``my_method`` in the class ``AwesomeClass``
- ``function("AwesomeNamespace\\my_function")`` will match the function ``my_function`` in the namespace ``AwesomeNamespace``

The ``param`` filter is also able to do some dereferencing:

- ``param($foo[bar])`` will get a match on the value corresponding to the ``bar`` key in the hashtable ``foo``.
  Remember that in PHP, almost every data structure is a hashtable. You can of course nest this like
  ``param($foo[bar][$object->array['123']][$batman])``.
- The ``var`` filter will walk the calltrace until it finds the variable name, or the end of the calltrace,
  allowing the filter to match global variables: ``.var("$_GET[\"param\"]")`` will match on the GET parameter ``param``.

The ``filename`` filter requires a leading ``/``, since paths are absolutes (like ``/var/www/mywebsite/lib/parse.php``).
If you would like to have only one configuration file for several vhost in different folders,
you can use the ``filename_r`` directive to match on the filename (like ``/lib/parse\.php``).

For clarity, the presence of the ``allow`` or ``drop`` action is **mandatory**.

.. warning::

  When you're writing rules, please do keep in mind that **the order matters**.
  For example, if you're denying a call to ``system()`` and then allowing it in a
  more narrowed way later, the call will be denied,
  because it'll match the deny first.

If you're paranoid, we're providing a `php script
<https://github.com/nbs-system/snuffleupagus/blob/master/scripts/generate_rules.php>`__
to automatically generate hash of files containing dangerous functions, and
blacklisting them everywhere else.

Limitations
^^^^^^^^^^^

It's currently not possible to:

- Hook every `language construct <https://secure.php.net/manual/en/reserved.keywords.php>`__,
  because each of them requires a specific implementation.
- Hook on the return value of user-defined functions
- Use extra-convoluted rulesfor matching, like ``${$A}$$B->${'}[1]``, because if you're writing
  things like this, odds are that you're doing something wrong anyway.


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

  sp.disable_function.function("system").param("$cmd").value("id").allow();
  sp.disable_function.function("system").param("$cmd").value_r("^ping").drop().simulation();
  sp.disable_function.function("system").param("$cmd").drop();

Miscellaneous examples
""""""""""""""""""""""

.. literalinclude:: ../../config/examples.ini
   :language: python
