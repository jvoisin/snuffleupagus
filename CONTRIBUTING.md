## Contributing

First off, thank you for considering contributing to snuffleupagus.

### 1. Where do I go from here?

If you've noticed a bug or have a question,
look at the [faq](https://snuffleupagus.readthedocs.io/faq.html) and
[search the issue tracker](https://github.com/nbs-system/snuffleupagus/issues)
to see if someone else has already created a ticket. If not, go ahead and
[make one](https://github.com/nbs-system/snuffleupagus/issues/new)!

### 2. Fork & create a branch

If this is something you think you can fix,
then [fork snuffleupagus](https://help.github.com/articles/fork-a-repo) and
create a branch with a descriptive name.

A good branch name would be (where issue #325 is the ticket you're working on):

```sh
git checkout -b 325-kill-sql-injections
```

### 3. Get the test suite running

Just type `make coverage` or `make debug`, the testsuite should be run
automatically.

Please add tests if you're fixing a bug or adding a new feature: we do have a
[high coverage](https://coveralls.io/github/nbs-system/snuffleupagus?branch=master)
(functions, lines and branches), and intend to keep it that way.

#### 3.3 Debugging failures in the test suite

If your changes have introduced run-time failures in the test-suite, you can
easily troubleshoot them by inspecting the files that
[php has generated](https://qa.php.net/write-test.php#analyzing-failing-tests)
for this purpose.

A nice trick is to edit the `.sh` file to prepend `gdb --args` to it before
launching it, in order to run the failing test inside GDB.


### 4. Did you find a bug?

* **Ensure the bug was not already reported** by
  [searching all issues](https://github.com/nbs-system/snuffleupagus/issues?q=).
* If you're unable to find an open issue addressing the problem,
  [open a new one](https://github.com/nbs-system/snuffleupagus/issues/new).
  Be sure to include a **title and clear description**,
  as much relevant information as possible, and a **code sample**
  or an **executable test case** demonstrating the expected behavior that is not
  occurring.


### 5. Get the style right

Your patch should follow the same conventions & pass the same code quality
checks as the rest of the project. We're using [clang-format](http://clang.llvm.org/docs/ClangFormat.html) to
ensure a consistent code-style. Please run it with `clang-format -style=google`
before committing, or even better, use a [pre-commit hook](https://github.com/andrewseidl/githook-clang-format).

### 6. Make a Pull Request

At this point, you should switch back to your master branch and make sure it's
up to date with our upstream master branch:

```sh
git remote add upstream git@github.com:nbs-system/snuffleupagus.git
git checkout master
git pull upstream master
```

Then update your feature branch from your local copy of master, and push it!

```sh
git checkout 325-kill-sql-injections
git rebase master
git push --set-upstream origin 325-kill-sql-injections
```

Finally, go to GitHub and [make a Pull Request](https://help.github.com/articles/creating-a-pull-request) :D

Travis CI will [run our test suite](https://travis-ci.org/nbs-system/snuffleupagus)
against all supported PHP versions. We care about quality, so your PR won't be
merged until all tests pass. It's unlikely, but it's possible that your changes
pass tests in one PHP version but fail in another. In that case, you'll have to
setup your development environment to use the problematic PHP version, and
investigate what's going on!

### 7. Keeping your Pull Request updated

If a maintainer asks you to "rebase" your PR, they're saying that a lot of code
has changed, and that you need to update your branch so it's easier to merge.

To learn more about rebasing in Git, there are a lot of [good](http://git-scm.com/book/en/Git-Branching-Rebasing)
[resources](https://help.github.com/articles/interactive-rebase) but here's the suggested workflow:

```sh
git checkout 325-kill-sql-injections
git pull --rebase upstream master
git push --force-with-lease 325-kill-sql-injections
```

### 8. Merging a PR (maintainers only)

A PR can only be merged into master by a maintainer if:

1. It is passing CI.
2. It has been approved by at least one maintainer. If it was a maintainer who
   opened the PR, only one extra approval is needed.
3. It has no requested changes.
4. It is up to date with current master.

Any maintainer is allowed to merge a PR if all of these conditions are met.

### 9. Shipping a release (maintainers only)

Maintainers need to do the following to push out a release:

1. Make sure all pull requests are in and that changelog is current
2. Update `snuffleupagus.h` file and changelog with new version number
3. Create a tag for that release:

  ```sh
  git checkout master
  git pull origin master
  make coverage
  git tag -s $MAJOR.$MINOR.$PATCH
  git push --tags
  ```

4. Do the *secret release dance*
