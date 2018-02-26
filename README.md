<h1 align="center">
  <br>
  <a href="https://snuffleupagus.readthedocs.io/">
		<img src="https://github.com/nbs-system/snuffleupagus/raw/improve_readme/doc/source/_static/sp.png" alt="Snuffleupagus' logo" width="200"></a>
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
    <img src="https://img.shields.io/twitter/url/https/twitter.com/sp_php.svg?style=social&label=Follow%20%40sp_php"
				 alt="twitter">
  </a>
</p>

<p align="center">
  <a href="#key-features">Key Features</a> •
  <a href="#download">Download</a> •
	<a href="https://snuffleupagus.readthedocs.io/">Documentation</a> •
  <a href="https://github.com/nbs-system/snuffleupagus/blob/master/LICENSE">License</a>
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
	* Unserialize-based code execution
	* `mail`-based code execution
	* Cookie-stealing XSS
	* File-upload based code execution
	* Weak PRNG
	* XXE
* Hardening features
	* Automatic `secure` and `samesite` flag for cookies
	* Bundled set of rule to detect post-compromissions behaviours
	* Global *strict mode*
	* Preventing writeable files execution
	* Whitelist/blacklist for `eval`
	* Request dumping capability

## Download

We've got a [download
page](https://snuffleupagus.readthedocs.io/download.html), where you can find
packages for your distribution, but you can of course just `git clone` this
repo, or check the releases on [github](https://github.com/nbs-system/snuffleupagus/releases).

## Documentation

We've got a [comprehensive website](https://snuffleupagus.readthedocs.io/) with
all the documentation that you could possibly wish for. You can of course
[build it yourself](https://github.com/nbs-system/snuffleupagus/tree/master/doc).
