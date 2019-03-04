#!/bin/bash
set -e
search_path="/Users/hdang/working/3p/src/"

# Create an indexed database for mlocate
mlocate_db=".database"
rm -f $mlocate_db
mupdatedb $search_path -d "$mlocate_db"

# Create an indexed database for GNU locate
locate_db="locate.db"
rm -f $locate_db
gupdatedb --localpaths=$search_path --output=$locate_db
