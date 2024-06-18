<h1 align="center">
  <br>
  <a href="https://snuffleupagus.readthedocs.io/">
    <img src="https://github.com/jvoisin/snuffleupagus/raw/master/doc/source/_static/sp.png" alt="Snuffleupagus' logo" width="200"></a>
  <br>
  Snuffleupagus
  <br>
</h1>

<h4 align="center">Security module for php7 and php8 - Killing bugclasses and virtual-patching the rest!</h4>

<p align="center">
  <a href="https://github.com/jvoisin/snuffleupagus/actions/workflows/distributions_php7.yml">
    <img src="https://github.com/jvoisin/snuffleupagus/actions/workflows/distributions_php7.yml/badge.svg"
         alt="Testing PHP7 on various Linux distributions" />
  </a>
  <a href="https://github.com/jvoisin/snuffleupagus/actions/workflows/distributions_php8.yml">
    <img src="https://github.com/jvoisin/snuffleupagus/actions/workflows/distributions_php8.yml/badge.svg"
         alt="Testing PHP8 on various Linux distributions" />
  </a>
  <a href="https://scan.coverity.com/projects/jvoisin-snuffleupagus">
    <img src="https://scan.coverity.com/projects/13821/badge.svg?flat=1"
         alt="Coverity">
  </a>
  <a href="https://bestpractices.coreinfrastructure.org/projects/1267">
      <img src="https://bestpractices.coreinfrastructure.org/projects/1267/badge"
           alt="CII Best Practises">
  </a>
  <a href="http://snuffleupagus.readthedocs.io/?badge=latest">
    <img src="https://readthedocs.org/projects/snuffleupagus/badge/?version=latest"
         alt="readthedocs.org">
  </a>
  <a href="https://coveralls.io/github/jvoisin/snuffleupagus?branch=master">
    <img src="https://coveralls.io/repos/github/jvoisin/snuffleupagus/badge.svg?branch=master"
         alt="coveralls">
  </a>
  <a href="https://twitter.com/dustriorg">
    <img src="https://img.shields.io/badge/twitter-follow-blue.svg"
         alt="twitter">
  </a>
  <a href="https://repology.org/project/php:snuffleupagus/versions">
    <img src="https://repology.org/badge/tiny-repos/php:snuffleupagus.svg"
         alt="Packaging status">
  </a>
  <a href="https://github.com/jvoisin/snuffleupagus">
    <img src="https://github.com/jvoisin/snuffleupagus/actions/workflows/codeql-analysis.yml/badge.svg"
         alt="CodeQL">
  </a>
</p>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#download">Download</a> •
  <a href="#examples">Examples</a> •
  <a href="https://snuffleupagus.readthedocs.io/">Documentation</a> •
  <a href="https://github.com/jvoisin/snuffleupagus/blob/master/LICENSE">License</a> •
  <a href="#thanks">Thanks</a>
</p>

Snuffleupagus is a [PHP 7+ and 8+](https://secure.php.net/) module designed to
drastically raise the cost of attacks against websites, by killing entire bug
classes. It also provides a powerful virtual-patching system, allowing
administrator to fix specific vulnerabilities and audit suspicious behaviours
without having to touch the PHP code.

## Key Features

* No [noticeable performance impact](https://dustri.org/b/snuffleupagus-030-dentalium-elephantinum.html)
* Powerful yet simple to write virtual-patching rules
* Killing several classes of vulnerabilities
  * [Unserialize-based](https://www.owasp.org/images/9/9e/Utilizing-Code-Reuse-Or-Return-Oriented-Programming-In-PHP-Application-Exploits.pdf) code execution
  * [`mail`-based]( https://blog.ripstech.com/2016/roundcube-command-execution-via-email/ ) code execution
  * Cookie-stealing [XSS]( https://en.wikipedia.org/wiki/Cross-site_scripting )
  * File-upload based code execution
  * Weak PRNG
  * [XXE]( https://en.wikipedia.org/wiki/XML_external_entity_attack )
  * Filter based remote code execution and assorted shenanigans
* Several hardening features
  * Automatic `secure` and `samesite` flag for cookies
  * Bundled set of rules to detect post-compromissions behaviours
  * Global [strict mode]( https://secure.php.net/manual/en/migration70.new-features.php#migration70.new-features.scalar-type-declarations) and type-juggling prevention
  * Whitelisting of [stream wrappers](https://secure.php.net/manual/en/intro.stream.php)
  * Preventing writeable files execution
  * Whitelist/blacklist for `eval`
  * Enforcing TLS certificate validation when using [curl](https://secure.php.net/manual/en/book.curl.php)
  * Request dumping capability
* A relatively sane code base:
  * A [comprehensive](https://coveralls.io/github/jvoisin/snuffleupagus?branch=master) test suite close to 100% coverage
  * Every commit is tested on [several distributions](https://gitlab.com/jvoisin/snuffleupagus/pipelines)
  * An `clang-format`-enforced code style
  * A [comprehensive documentation](https://snuffleupagus.rtfd.io)
  * Usage of [coverity](https://scan.coverity.com/projects/jvoisin-snuffleupagus), codeql, [scan-build](https://clang-analyzer.llvm.org/scan-build.html), …

## Download

We've got a [download
page](https://snuffleupagus.readthedocs.io/download.html), where you can find
packages for your distribution, but you can of course just `git clone` this
repo, or check the releases on [github](https://github.com/jvoisin/snuffleupagus/releases).

## Examples

We're providing [various example rules](https://github.com/jvoisin/snuffleupagus/tree/master/config),
that are looking like this:

```python
# Harden the `chmod` function
sp.disable_function.function("chmod").param("mode").value_r("^[0-9]{2}[67]$").drop();

# Mitigate command injection in `system`
sp.disable_function.function("system").param("command").value_r("[$|;&`\\n]").drop();
```

Upon violation of a rule, you should see lines like this in your logs:

```python
[snuffleupagus][0.0.0.0][disabled_function][drop] The execution has been aborted in /var/www/index.php:2, because the return value (0) of the function 'strpos' matched a rule.
```

## Documentation

We've got a [comprehensive website](https://snuffleupagus.readthedocs.io/) with
all the documentation that you could possibly wish for. You can of course
[build it yourself](https://github.com/jvoisin/snuffleupagus/tree/master/doc).

## Thanks

Many thanks to:

- The [Suhosin project](https://suhosin.org) for being a __huge__ source of inspiration
- [NBS System](https://www.nbs-system.com) for initially sponsoring the development
- [Suhosin-ng](https://github.com/sektioneins/suhosin-ng) for their
  [experimentations](https://github.com/sektioneins/suhosin-ng/wiki/News)
  and [contributions](https://github.com/jvoisin/snuffleupagus/commits?author=bef),
  as well as [NLNet](https://nlnet.nl/project/Suhosin-NG/) for sponsoring it
- All [our contributors](https://github.com/jvoisin/snuffleupagus/graphs/contributors)

