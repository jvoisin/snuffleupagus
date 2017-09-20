# ALPHA SOFTWARE : DO NOT USE IN PRODUCTION #

Snuffleupagus is a [PHP 7+](https://secure.php.net/) module designed to
drastically raising the cost of attacks against website, by killing entire bug
classes, and also providing a powerful virtual-patching system, allowing
administrator to fix specific vulnerabilities and audit suspicious behaviours
without having to touch the PHP code.

You can read its documentation [here](https://snuffleupagus.readthedocs.io/).

# Code style

We're using [clang-format](http://clang.llvm.org/docs/ClangFormat.html) to
ensure a consistent code-style. Please run it with `clang-format -style=google`
before committing, or even better, use a [pre-commit hook](https://github.com/andrewseidl/githook-clang-format)
