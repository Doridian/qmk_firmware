#!/bin/sh
set -e

git checkout master
git pull
git checkout main
git pull

git rebase master

git push -f

