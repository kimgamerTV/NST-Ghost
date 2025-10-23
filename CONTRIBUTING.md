# Contributing to NST

First off, thank you for considering contributing to NST! It's people like you that make NST such a great tool.

## Where do I go from here?

If you've noticed a bug or have a feature request, [make one](https://github.com/user/repo/issues/new)! It's generally best if you get confirmation of your bug or approval for your feature request this way before starting to code.

## Fork & create a branch

If you decide to fix a bug or implement a feature, great! Fork the repository and create a branch with a descriptive name.

A good branch name would be (where issue #38 is the ticket you're working on):

```
git checkout -b 38-add-awesome-new-feature
```

## Get the test suite running

Make sure you can get the test suite running. If you have any trouble, you can ask for help in the issue you're working on.

## Implement your fix or feature

At this point, you're ready to make your changes! Feel free to ask for help; everyone is a beginner at first :smile_cat:

## Make a Pull Request

At this point, you should switch back to your master branch and make sure it's up to date with the latest upstream version of master.

```
git remote add upstream git@github.com:user/repo.git
git checkout master
git pull upstream master
```

Then update your feature branch from your local copy of master, and push it!

```
git checkout 38-add-awesome-new-feature
git rebase master
git push --force-with-lease origin 38-add-awesome-new-feature
```

Finally, go to GitHub and make a Pull Request.

## Keeping your Pull Request updated

If a maintainer asks you to "rebase" your PR, they're saying that a lot of code has changed, and that you need to update your branch so it's easier to merge.

To learn more about rebasing and merging, check out this guide: [About Git rebase](https://docs.github.com/en/get-started/using-git/about-git-rebase).

## Merging a PR (for maintainers)

A PR can only be merged by a maintainer if it has at least one approval, and all status checks have passed.

For more details, see the [GitHub documentation on merging a pull request](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/incorporating-changes-from-a-pull-request/merging-a-pull-request).
