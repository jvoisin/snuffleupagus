<h1 align="center">
  <br>
  <a href="https://snuffleupagus.readthedocs.io/">
		<img src="https://github.com/nbs-system/snuffleupagus/raw/master/doc/source/_static/sp.png" alt="Snuffleupagus' logo" width="200"></a>
  <br>
  Snuffleupagus
  <br>
</h1>

<h4 align="center">Security module for php7 - Killing bugclasses and virtual-patching the rest!.</h4>

<p align="center">
  <a href="https://travis-ci.org/nbs-system/snuffleupagus">
    <img src="https://travis-ci.org/nbs-system/snuffleupagus.svg?branch=master"
         alt="Travis-ci">
  </a>
  <a href="https://scan.coverity.com/projects/nbs-system-snuffleupagus">
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
  <a href="https://coveralls.io/github/nbs-system/snuffleupagus?branch=master">
    <img src="https://coveralls.io/repos/github/nbs-system/snuffleupagus/badge.svg?branch=master"
				 alt="coveralls">
  </a>
  <a href="https://twitter.com/sp_php">
    <img src="https://img.shields.io/badge/twitter-follow-blue.svg"
				 alt="twitter">
  </a>
</p>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#download">Download</a> •
  <a href="#examples">Examples</a> •
	<a href="https://snuffleupagus.readthedocs.io/">Documentation</a> •
  <a href="https://github.com/nbs-system/snuffleupagus/blob/master/LICENSE">License</a> •
	<a href="#thanks">Thanks</a>
</p>

Snuffleupagus is a [PHP 7+](https://secure.php.net/) module designed to
drastically raise the cost of attacks against websites, by killing entire bug
classes. It also provides a powerful virtual-patching system, allowing
administrator to fix specific vulnerabilities and audit suspicious behaviours
without having to touch the PHP code.

## Key Features

* Close to zero performance impact
* Powerful yet simple to write virtual-patching rules
* Killing several classes for vulnerabilities
	* [Unserialize-based](https://www.owasp.org/images/9/9e/Utilizing-Code-Reuse-Or-Return-Oriented-Programming-In-PHP-Application-Exploits.pdf) code execution
	* [`mail`-based]( https://blog.ripstech.com/2016/roundcube-command-execution-via-email/ ) code execution
	* Cookie-stealing [XSS]( https://en.wikipedia.org/wiki/Cross-site_scripting )
	* File-upload based code execution
	* Weak PRNG
	* [XXE]( https://en.wikipedia.org/wiki/XML_external_entity_attack )
* Hardening features
	* Automatic `secure` and `samesite` flag for cookies
	* Bundled set of rule to detect post-compromissions behaviours
	* Global [strict mode]( https://secure.php.net/manual/en/migration70.new-features.php#migration70.new-features.scalar-type-declarations) and type-juggling prevention
	* Whitelisting of [stream wrappers](https://secure.php.net/manual/en/intro.stream.php)
	* Preventing writeable files execution
	* Whitelist/blacklist for `eval`
	* Request dumping capability

## Download

We've got a [download
page](https://snuffleupagus.readthedocs.io/download.html), where you can find
packages for your distribution, but you can of course just `git clone` this
repo, or check the releases on [github](https://github.com/nbs-system/snuffleupagus/releases).

## Examples

We're providing [various example rules](https://github.com/nbs-system/snuffleupagus/tree/master/config),
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
[build it yourself](https://github.com/nbs-system/snuffleupagus/tree/master/doc).

## Thanks

Many thanks to the [Suhosin project](https://suhosin.org) for being a __huge__
source of inspiration, and to all [our
contributors](https://github.com/nbs-system/snuffleupagus/graphs/contributors).
