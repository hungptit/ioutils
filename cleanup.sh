#!/bin/bash
# This script will automatically cleanup all merged branches.
# Note: This bash script required fgrep command.
set -e
set -x

# Check out and update the master branch
git checkout master
git pull

# Cleanup merged branches
merged_branches=$(git branch --merge | grep --stdin --invert-match 'master')
for branch in $merged_branches; do
    echo "Delete branch '$branch'"
    sleep 1
    git branch -d "$branch"
done

set +x
