FAQ
===

General
-------

What is Snuffleupagus?
""""""""""""""""""""""

Snuffleupagus is a `PHP7+ <http://php.net/manual/en/migration70.php>`_
module designed to drastically raise the cost of attacks against websites. This is achieved
by killing entire bug classes and providing a powerful virtual-patching system,
allowing the administrator to fix specific vulnerabilities without having to touch the PHP code.


Where does the name *Snuffleupagus* come from?
""""""""""""""""""""""""""""""""""""""""""""""

  Aloysius Snuffleupagus, more commonly known as Mr. Snuffleupagus,
  or Snuffy for short, is one of the characters on Sesame Street,
  the educational television program for young children.

  He was created as a woolly mammoth without tusks or (visible) ears,
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

The project started at `NBS System <https://nbs-system.com/en/>`__,
a web hosting company (meaning that we're dealing with PHP code all day long),
with a strong focus on security. We do have several layers of hardening
(`kernel <https://grsecurity.net/>`_, `WAF <https://naxsi.org>`_,
`IDS <https://en.wikipedia.org/wiki/Intrusion_detection_system>`_, etc),
but we had nothing for PHP7.

Nowadays, Snuffleupagus is maintained by Julien (`jvoisin <https://dustri.org>`__) Voisin.


Why not Suhosin?
""""""""""""""""

We're huge fans of `Suhosin <https://suhosin.org>`_, unfortunately:

- it doesn't work very well on PHP7
- it has some oudated features and misses new ones
- it doesn't cope very well with our various industrialization needs
- it has some shortcomings by design

We're using the `disable_function <https://secure.php.net/manual/en/ini.core.php#ini.disable-functions>`_
directive, but unfortunately, it doesn't provide enough usable granularity (guess how many CMSs are using
the `system <https://secure.php.net/manual/en/function.system.php#refsect1-function.system-notes>`_
function to perform various mandatory maintenance tasks).

This is why we decided to write our own hardening module, in the spirit of Suhosin,
with virtual-patching support, as well as other cool new features.


What license is Snuffleupagus released under and why?
"""""""""""""""""""""""""""""""""""""""""""""""""""""

Snuffleupagus is licensed under the `LGPL <https://www.gnu.org/copyleft/lesser.html>`_
was developed by the fine people from `NBS System <https://nbs-system.com/>`__,
and is maintained by Julien (jvoisin) Voisin.

We chose the LGPL because we don't care that much how you're using Snuffleupagus,
but we'd like to force people to make their improvements/contributions
available to everyone.

The complete license text is shipped with the sources and can be found under ``LICENSE``.

For compatibility with older PHP versions, some original PHP source code was copied or ported back to older versions.
This source code resides in ``src/sp_php_compat.c`` and ``src/sp_php_compat.h`` and retains its original license
`The PHP License, version 3.01 <https://www.php.net/license/3_01.txt>`_, also included with the sources as ``PHP_LICENSE``.


What is the different between Snuffleupagus and a (WAF) like ModSecurity?
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

`ModSecurity <https://modsecurity.org/>`__ and other `Web Application
Firewalls (WAF) <https://en.wikipedia.org/wiki/Web_application_firewall>`__ are
working by inspecting the http traffic. Snuffleupagus being a PHP module, is
operating directly inside your website's code, with a lesser overhead, as well
as a better understanding of what is currently happening inside your
application.


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

This project has been floating around since early 2016 and we did the first commit
the 28:sup:`th` of December of the same year. It's currently stable,
and is usable and used in production.

Are you saying that PHP isn't secure?
"""""""""""""""""""""""""""""""""""""

We don't like PHP's approach of security; namely (sometimes) adding warnings
in the documentation and trusting the developer to not do any mistake,
instead of focusing on the root cause and killing the
bug class once and for all.

Moreover, it seems that the current attitude toward security in the PHP world
is to `blame the user <https://externals.io/message/100147>`_ instead of acknowledging
issues, as stated in their `documentation <https://wiki.php.net/security#not_a_security_issue>`_.
We do think that a security issue that "requires the use of code or settings known to be insecure"
is still a security issue, and should be treated as such.

We don't have the pretension to state that Snuffleupagus will magically solve
all your security issues, but we believe that it might definitely help.


Sounds great, but is it working?
""""""""""""""""""""""""""""""""

We've been using it in production since a couple of years, and it thwarted
numerous known and unknown attacks. If you want some evidences, one of the
developer published in June 2019 a `blogpost
<https://dustri.org/b/snuffleupagus-versus-recent-high-profile-vulnerabilities.html>`__
showcasing how efficient Snuffleupagus was versus *major* web
vulnerabilities from 2018/2019.


Why should I send you bugs, security issues and patches?
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""
Snuffleupagus is an open-source security software, by reporting (or fixing)
bugs, or implementing new features, you are helping others to protect themselves.

We're also firm believers in the *Beerbounty* system:
we are happy to offer you beers when/if we ever meet if you helped the project in
any way. If you don't like beer, we're sure that we'll find something else,
don't worry.


Installation and configuration
------------------------------

Can snuffleupagus break my application?
"""""""""""""""""""""""""""""""""""""""
Yes.

Some options won't break anything, like :ref:`harden-rand <harden-rand-feature>`,
but some like :ref:`global_strict <global-strict-feature>`
or overly-restrictive :ref:`virtual-patching<virtual-patching-feature>`
rules might pretty well break your website.
It's up to you to configure Snuffleupagus accordingly to your needs.

You can also enable the ``simulation`` mode on features that you're not sure about,
to see what snuffleupagus would do to your application, before activating them for good.

How can I find out the problem when my application breaks?
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

By checking the logs; Snuffleupagus systematically prefix them with ``[snuffleupagus]``.


Does Snuffleupagus run on Windows?
""""""""""""""""""""""""""""""""""
No idea, feel free to `try <https://github.com/jvoisin/snuffleupagus/issues/2>`_.


Does Snuggleupagus run on `HHVM <http://hhvm.com/>`_?
"""""""""""""""""""""""""""""""""""""""""""""""""""""
No it doesn't, since `HHVM's API <https://github.com/facebook/hhvm/wiki/Extension-API>`_
is really different from PHP7's one. We're not currently planning to rewrite
Snuffleupagus to support it.

Will Snuffleupagus run on my old PHP 5?
"""""""""""""""""""""""""""""""""""""""
No.

Since PHP5 `is deprecated since the end of 2018 <http://php.net/supported-versions.php>`_,
you should think about moving to PHP7 or newer. You can (and should) use
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

Please do send a mail to [Julien (jvoisin) Voisin](https://dustri.org) should
you find a security issue.


I found a bug. How can I report it?
"""""""""""""""""""""""""""""""""""
We do have an issue tracker on `Github <https://github.com/jvoisin/snuffleupagus/issues>`_.
Please make sure to include as much information as possible when reporting your issue,
such as your operating system, your version of PHP, your version of Snuffleupagus,
your logs, the problematic PHP code, the request, a brief description, … long story short,
give us everything that you can.

If you're feeling extra-nice, you can try to :ref:`debug it yourself <debug>`,
it's not that hard.

Where can I find even more help?
""""""""""""""""""""""""""""""""
The :doc:`configuration page <config>` might be what you're looking for.
If you're adventurous, you can also check the `issue tracker <https://github.com/jvoisin/snuffleupagus/issues/?q=is%3Aissue>`_
(make sure to check the `closed issues <https://github.com/jvoisin/snuffleupagus/issues?q=is%3Aissue+is%3Aclosed>`_ too).


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

Nop'ing function execution
""""""""""""""""""""""""""

Snuffleupagus can be configured to either *allow* or *drop* the execution of
particular functions and optionally *log* and *dump* them, but it doesn't
provide any mechanism to *nop* their execution.

We thought about adding this, but didn't for several reasons:

- What should the return value of a *nop'ed* function be?
- It would add confusion between ``drop``, ``nop`` and ``log``.
- Usually, when a specific function is called, either it's a dangerous one
  and you want to stop the execution immediately, or you want to let it
  continue and log it. There isn't really any middle-ground, or at least we
  failed to find any.
