#!/bin/sh
set -e

git checkout master
git pull
git checkout mine
git pull

git rebase master

git push -f

