#!/bin/bash

# Cache the current branch name
current_branch=`git branch | grep '^*' | tr '*' ' '`

# Update develop branch
git checkout develop
git pull

# TODO: Update other branches

# Revert to the current branch
git checkout $current_branch
