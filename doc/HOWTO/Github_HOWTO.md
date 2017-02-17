# Github-HOWTO

Reference: https://help.github.com/articles/syncing-a-fork/

## Set up your repository

* Fork our github repo https://github.com/kbumsik/WolfieMouse
* Clone your forked repo. e.g:

> $ git clone https://github.com/{your-id}/WolfieMouse

* Add our repo as a new remote `upstream`

> $ git remote add upstream https://github.com/kbumsik/WolfieMouse

## Make new change input your repository

Check what you changed

> $ git status

Add your changes into staged mode

> $ git add {files-you-changed} or  $ git add --all

Double check change

> $ git status

Commit change

> $ git commit -m "some message. e.g. what you changed"

Push it to your repository

> $ git push origin master

Then you can make a pull request later


## Update your repo with `upstream`

* Update your local repo with `upstream`

First get information of `upstream`

> $ git fetch upstream

next,

> $ git pull upstream master

or

> $ git rebase upstream/master

Then

> $ git submodule update

* push it to `origin` repo, your forked repository

> $ git push origin master

## Useful git commands

* `git log --decorate --all --graph`
* `git fetch upstream master`
* `git stash` and `git stash pop`
* `git diff <commit>`