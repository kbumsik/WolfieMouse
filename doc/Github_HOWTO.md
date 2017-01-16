# Github-HOWTO

Reference: https://help.github.com/articles/syncing-a-fork/

## Useful git commands

* `git log --decorate --all --graph`
* `git fetch upstream master`
* `git stash` and `git stash pop`
* `git diff <commit>`

## Set up your repository

1. Fork our github repo https://github.com/kbumsik/WolfieMouse
2. Clone your forked repo. e.g:
> $ git clone https://github.com/<your_id>/WolfieMouse
3. Add our repo as a new remote `upstream`
> $ git remote add upstream https://github.com/kbumsik/WolfieMouse

## Update your repo with `upstream`

1. Update your local repo with `upstream`
> $ git merge upstream/master

or
> $ git rebase upstream/master
2. push it to `origin` repo, your forked repository
> $ git push origin master
