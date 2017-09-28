FAQ
===

General
-------

What is Snuffleupagus?
""""""""""""""""""""""

Snuffleupagus is a `PHP7+ <http://php.net/manual/en/migration70.php>`_
module designed to drastically raising the cost of attacks against website,
by killing entire bug classes, and also providing a powerful virtual-patching system,
allowing administrator to fix specific vulnerabilities without having to touch the PHP code.


Where does the name *Snuffeupagus* comes from?
""""""""""""""""""""""""""""""""""""""""""""""

  Aloysius Snuffleupagus, more commonly known as Mr. Snuffleupagus, Snuffleupagus
  or Snuffy for short, is one of the characters on Sesame Street,
  the educational television program for young children.

  He was created as a woolly mammoth, without tusks or (visible) ears,
  and has a long thick pointed tail, similar in shape to that of a dinosaur
  or other reptile. He has long thick brown hair and a trunk, or "snuffle",
  that drags along the ground. He is Big Bird's best friend and
  has a baby sister named Alice. He also attends "Snufflegarten".

  --- `Wikipedia <https://en.wikipedia.org/wiki/Mr._Snuffleupagus>`_


Why is Snuffleupagus called Snuffleupagus?
""""""""""""""""""""""""""""""""""""""""""

Like PHP's `ElePHPant <https://secure.php.net/elephpant.php>`_,
we thought that using an elephant as a mascot would be a great idea.


Who are you and why did you write Snuffleupagus?
""""""""""""""""""""""""""""""""""""""""""""""""

We're working for `NBS System <https://nbs-system.com/en/>`__,
a web hosting company (meaning that we're dealing with PHP code all day long),
with a strong focus on security. We do have hardening
(`kernel <https://grsecurity.net/>`_, `WAF <https://naxsi.org>`_,
`IDS <https://en.wikipedia.org/wiki/Intrusion_detection_system>`_, …)
below the web stack, but most of the time, when a website is compromised,
it's either to send ads, spam, deface it, steal data, …
This is why we need to harden the website itself too, but we can't touch its
source code.

Why not Suhosin?
""""""""""""""""

We're huge fans of `Suhosin <https://suhosin.org>`_, unfortunately:

- it doesn't work very well on PHP 7
- it has some oudated features and misses new ones
- it doesn't cope very well with our various industrialization needs
- it has some shortcomings by design

We're using the `disable_function <https://secure.php.net/manual/en/ini.core.php#ini.disable-functions>`_
directive, but unfortunately, it doesn't provide enough usable granularity (guess how many CMS are using
the `system <https://secure.php.net/manual/en/function.system.php#refsect1-function.system-notes>`_
function to perform various mandatory maintenance tasks…).

This is why we decided to write our own hardening module, in the spirit of Suhosin,
via virtual-patching support, and other cool new features.

What license is Snuffleupagus under and why?
""""""""""""""""""""""""""""""""""""""""""""

Snuffleupagus is licensed under the `LGPL <https://www.gnu.org/copyleft/lesser.html>`_,
and is developed by the fine people from `NBS System <https://nbs-system.com/>`__.

We chose the LGPL because we don't care that much how you're using Snuffleupagus,
but we'd like to force people to make their improvements/contributions
available to everyone.

Should I use Snuffleupagus?
"""""""""""""""""""""""""""

Yes.

Even if you're not using the virtual-patching capabilities, Snuffleupagus comes
with various passive features that won't break your website while killing numerous vulnerabilities.

Please keep in mind that you are not only protecting yourself and your users/customers,
but also other people on the internet that might be attacked by your server if
it becomes compromised.

How mature is this project?
"""""""""""""""""""""""""""

This project was floating around since early 2016, and we did the first commit
the 28ᵗʰ of December of the same year. We're currently in an alpha phase,
finding and fixing as much bugs as possible before the beta.

Are you saying that PHP isn't secure?
"""""""""""""""""""""""""""""""""""""

We don't like PHP's approach of security; namely (sometimes) adding warnings
in the documentation and trusting the developer to not do any mistake,
instead of focusing on the root cause, and killing the
bug class one for all.

Moreover, it seems that the current attitude toward security in the PHP world
is to `blame the user <https://externals.io/message/100147>`_ instead of acknowledging
issues, as stated in their `documentation <https://wiki.php.net/security#not_a_security_issue>`_.
We do think that an security issue that "requires the use of code or settings known to be insecure"
is still a security issue, and should be treated as such.

We don't have the pretention to state that Snuffleupagus will magically solve
all your security issues, but we believe that it might definitely help.

Why should you send us bugs, security issues and patches?
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""
Snuffleupagus is an open-source security software, by reporting (or fixing)
bugs, or implementing new features, you are helping others to protect themselves.

We're also firm believer in the *Beerbounty* system:
we are happy to offer you beers when/if we ever met if you helped the project in
any way. If you don't like beer, we're sure that we'll find something else,
don't worry.


Installation and configuration
------------------------------

Can snuffleupagus break my application?
"""""""""""""""""""""""""""""""""""""""
Yes.

Some options won't break anything, like :ref:`harden-rand <harden-rand-feature>`,
but some like :ref:`global_strict <global-strict-feature>`
or overly-restrictives :ref:`virtual-patching<virtual-patching-feature>`
rules might pretty well break your website.
It's up to you to configure Snuffleupaggus accordingly to your needs.

You can also enable the ``simulation`` mode on features that you're not sure about,
to see what would snuffleupagus do to your application, before activating them for good.

How can I find out the problem when my application breaks?
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

By checking the logs; Snuffleupagus systematically prefix them with ``[snuffleupagus]``.


Does Snuffleupagus run on Windows?
""""""""""""""""""""""""""""""""""
No idea, feel free to `try <https://github.com/nbs-system/snuffleupagus/issues/2>`_.


Will Snuffleupagus run on my old PHP 5?
"""""""""""""""""""""""""""""""""""""""
No.

Since PHP5 `will be deprecated at the end of 2018 <http://php.net/supported-versions.php>`_,
you should think about moving to PHP7 anyway. You can (and should) use
`Suhosin <https://suhosin.org>`_ in the meantime.

Help and support
----------------

I found a security issue
""""""""""""""""""""""""
If you believe you have found a security issue affecting Snuffleupagus,
then we would be more than happy to hear from you!

We promise to treat any reported issue seriously and,
if the investigation confirms it affects Snuffleupagus,
to patch it within a reasonable time,
release a public announcement that describes the issue,
discuss potential impact of the vulnerability,
reference applicable patches or workarounds,
and credit the discoverer.

Please send it us a mail to the ``security`` user,
on ``nbs-system.com``, using the gpg key
``498C46FF087EDC36E7EAF9D445414A82A9B22D78``:

::

    -----BEGIN PGP PUBLIC KEY BLOCK-----

    mQENBFnKHhoBCADaOa0MKEqRy0h2ohIzczblzkMQCbU9oD1HwJ1VkYnn7TGW2iKi
    NISxisExIXpy2Bn/pA27GiV0V/Do3NL6D9r0oOCrGR27muGM0N/dk9UMv7MWw8zv
    K8cO+Sa28s0cAv7r2ogUJj5YOo8D4wHEpE8424TE89V9+Qg/SaFCxKoELFP0c7wu
    mtsm0PnL65piZ1EB7lQo2gxg+8AV45MD1Y2rREMKUoZE23X+nXKsmEh9BFEPaU5M
    7WQp0NasqeMNoGhwfw9ttVAeLhkEkaTjW1PkNRIb7vrtV9KVb5uKucflfbOnDlzu
    tQ9U3tYto0mcSCRchAClfEmoSi/0mKyb5N6ZABEBAAG0NVNlY3VyaXR5IHRlYW0g
    b2YgTkJTIFN5c3RlbSA8c2VjdXJpdHlAbmJzLXN5c3RlbS5jb20+iQE3BBMBCAAh
    BQJZyh4aAhsDBQsJCAcCBhUICQoLAgQWAgMBAh4BAheAAAoJEEVBSoKpsi14jy0H
    /1/XB9THhvmG0ow81sld2Zx8qhnNed8VvYDS6mEjpDWNVPxENwDbnakEjisq1Hrb
    2UQPYCyQ5dekPNFVwQHIGXkX0eb1Ank+4esBJuEpQ2985tgNhJy5ZX+Imb5C8nZC
    90uYSN1UUg559nUsFeElOXSEH6tIXK/TvjsvMYoi2Ukl6lb7PbIU2fjLY9Iqv3QY
    32p8/Bl1fVKWbXOk0HDgJ6zA3Kr56QhZOLBkxjOa2XAnnIE76jZxUJ9qPCwWd1vW
    GFxtx1Y+eZriqHiC9CPe6aBWcIHaTXSu1WBbXrFu8/eCWw243Rxm8l9wgA/a7VWq
    WBfO45IhJUwh95naRpw8/4a5AQ0EWcoeGgEIAJtzSyyzfn2RX+BsyoRFANUpIgrV
    /9eohYQVNqK3AFthmq7Kjmt4+hszF5+0wCFmWwYqGnqk1/dsWmqpkXsJldEn6oPJ
    Bng+Dc67Yki2dR3TroAf95UmI08fhyM7TMXp8m46BPRRMzPNwalEeEm49Oclmfxb
    JsWWCChWVLWGz2xgPEAv3fPHqus7Rwz/WIl53l/qy1Wf0ewmjRpVEfnEMKBExtBK
    4kRxQ40LzUZ1SfpyGc3nMbswhevT7/klqrdJdCnlu67Y/IfRGxGZuNj1n1Dib3Hx
    zTBHo3Y2R3BB93Ix8dkbLaxLqFbOYVdijCgJklqUWhx7btpQ2xnZyzyCMuUAEQEA
    AYkBHwQYAQgACQUCWcoeGgIbDAAKCRBFQUqCqbIteFRvB/9u3Mae8n8ELrJKOn+P
    PEbWjutObIuTplvY4QcbnNb9dsgsKryamp4CFJsA5XuitPpC31GDMXBZO5/LLOuH
    HoMaXFJdic0NToL/3REhu+aZkNIU6S/iaPRNVhkSV4lwQsvncz+nBaiDUJjyfJm2
    kEjVcRTM8yqzcNo/9Gn0ts+XCUqRj7+S1M4Bj3NySoO/w2n+7OLbIAj+wQZcj3Gf
    5QhBYaY4YaFxrJE0IZxyXGHw8xhKR6AN+u4TO7LRCW+cWV/sHWir1MXieJoEG8+R
    W/BhrB0Rz5uxOXMoGCCD2TUiHq7zpuHGnYFVmAnHQZaaQxXve4VrcmznxgpV8lpW
    mZug
    =+eIv
    -----END PGP PUBLIC KEY BLOCK-----

I found a bug. How can I report it?
"""""""""""""""""""""""""""""""""""
We do have an issue tracker on `Github <https://github.com/nbs-system/snuffleupagus/issues>`_.
Please make sure to include as much information as possible when reporting your issue,
such as your operating system, your version of PHP 7, your version of snuffleupagus,
your logs, the problematic php code, the request, a brief description, … long story short,
give us everything that you can.

Where can I find even more help?
""""""""""""""""""""""""""""""""
The :doc:`configuration page <config>` might be what you're looking for.
If you're adventurous, you can also check the `issue tracker <https://github.com/nbs-system/snuffleupagus/issues/?q=is%3Aissue>`_
(make sure to check the `closed issues <https://github.com/nbs-system/snuffleupagus/issues?q=is%3Aissue+is%3Aclosed>`_ too).

I need professional support for my company.
"""""""""""""""""""""""""""""""""""""""""""
Contact `NBS System <https://nbs-system.com>`_.

Unimplemented mitigations and abandoned ideas
---------------------------------------------

Constant time comparisons
"""""""""""""""""""""""""
We didn't manage to perform time-based side-channel attacks on strings 
against real world PHP application, and the results that we gathered on
tailored test cases weren't concluding: for simplicity's sake, we chose
to not implement a mitigation against this class of attacks.

We would be happy to be proven wrong, and reconsider implementing this feature,
if someone can manage to get better results than us.

The possibility of having this natively in PHP has
`been discussed <https://marc.info/?l=php-internals&m=141692988212413&w=2>`_,
but as 2017, nothing has been merged yet.
