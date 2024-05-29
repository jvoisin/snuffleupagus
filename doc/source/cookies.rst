.. _cookie-encryption-page:

Cookies
=======

Some cookies-related features might prevent other extensions from hooking
the `setcookie <https://secure.php.net/manual/en/function.setcookie.php>`__
function. Pay attention to the loading order of your extensions in this case.

auto_cookie_secure
""""""""""""""""""
 
:ref:`auto_cookie_secure <auto-cookie-secure-feature>`, disabled by default,
will automatically mark cookies as `secure
<https://en.wikipedia.org/wiki/HTTP_cookie#Secure_cookie>`_ when the web page
is requested over HTTPS.

It can either be ``enabled`` or ``disabled``.

::

  sp.auto_cookie_secure.enable();
  sp.auto_cookie_secure.disable();

cookie_samesite
"""""""""""""""
 
:ref:`samesite <samesite-feature>`, disabled by default, adds the `samesite
<https://tools.ietf.org/html/draft-west-first-party-cookies-07>`_ attribute to
cookies. It `prevents CSRF <https://www.owasp.org/index.php/SameSite>`_ but is
not implemented by `all web browsers <https://caniuse.com/#search=samesite>`_
yet. Note that this is orthogonal to `PHP7.3+ SameSite support
<https://wiki.php.net/rfc/same-site-cookie>`__.

It can either be set to ``strict`` or ``lax``:

- The ``lax`` attribute prevents cookies from being sent cross-domain for
  "dangerous" methods, like ``POST``, ``PUT`` or ``DELETE``.

- The ``strict`` one prevents any cookies from being sent cross-domain.

::

  sp.cookie.name("cookie1").samesite("lax");
  sp.cookie.name_r("^cookie[0-9]+").samesite("lax");
  sp.cookie.name("cookie2").samesite("strict");;


Cookie encryption
"""""""""""""""""
   
The encryption is done via the `tweetnacl library <https://tweetnacl.cr.yp.to/>`_,
thus using `curve25519 <https://en.wikipedia.org/wiki/Curve25519>`__, `xsalsa20 <https://en.wikipedia.org/wiki/Salsa20#ChaCha_variant>`__ and `poly1305 <https://en.wikipedia.org/wiki/Poly1305>`__ for the encryption. We chose this
library because of its portability, license (public-domain), simplicity and reduced size (a single `.h` and
`.c` file.).

The key is derived from multiple sources, such as:
 * The ``secret_key`` provided in the configuration in the ``sp.global.secret_key``
   option. It's recommended to use something like ``head -c 256 /dev/urandom | tr -dc
   'a-zA-Z0-9'`` as a value.
 * An optional environment variable, such as ``REMOTE_ADDR`` (remote IP address) or the *extended master secret* from TLS connections (`RFC7627 <https://tools.ietf.org/html/rfc7627>`_) in the ``sp.global.cookie_env_var`` option.
 * The `user-agent <https://en.wikipedia.org/wiki/User_agent>`__.


.. warning::

  To use this feature, you **must** set the :ref:`global.secret_key <config_global>` variable
  and **should** set the :ref:`global.cookie_env_var <config_global>` one too.
  This design decision prevents an attacker from
  `trivially bruteforcing <https://www.idontplaydarts.com/2011/11/decrypting-suhosin-sessions-and-cookies/>`_
  or re-using session cookies.
  If the simulation mode isn’t specified in the configuration, snuffleupagus will drop any request that it was unable to decrypt.

Since PHP doesn't handle session cookie and non-session cookie in the same way,
so does Snuffleupagus.


Session cookie
..............

For the session cookie, the encryption happens server-side: Nothing is
encrypted in the cookie: neither the cookie's name (usually ``PHPSESSID``) nor
its content (the session's name).  What is in fact encrypted, is the session's
content, on the server (usually stored in ``/tmp/sess_<XXXX>`` files).

:ref:`Session encryption <cookie-encryption-feature>`, disabled by default, will activate transparent session encryption.
It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.

::

  sp.session.encrypt();
  sp.session.simulation();


Non-session cookie
..................

For the non-session cookie, the cookie's name is left untouched, only its value is encrypted.

:ref:`Cookie encryption <cookie-encryption-feature>`, disabled by default, will activate transparent encryption of specific cookies.

It can either be ``enabled`` or ``disabled`` and can be used in ``simulation`` mode.

::

  sp.cookie.name("my_cookie_name").encrypt();
  sp.cookie.name("another_cookie_name").encrypt();


Removing the user-agent part
............................

Some web browser extensions, such as `uMatrix <https://github.com/gorhill/uMatrix/wiki>`__
might be configured to change the user-agent on a regular basis. If you think that
some of your users might be using configurations like this, you might want to disable
the mixing of the user-agent in the cookie's encryption key. The simplest way to do
so is to set the environment variable ``HTTP_USER_AGENT`` to a fixed value before passing
it to your php process.

We think that this use case is too exotic to be worth implementing as a
proper configuration directive.

.. _env-var-config:

Choosing the proper environment variable
........................................

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
