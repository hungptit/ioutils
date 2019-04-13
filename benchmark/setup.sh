#!/bin/bash
set -e
search_path="${HOME}/working/3p/src/"

# Create an indexed database for fast-locate
fast_locate_db=".database"
rm -f $fast_locate_db
mupdatedb $search_path -d "$fast_locate_db"

# Create an indexed database for GNU locate
locate_db="locate.db"
rm -f $locate_db
gupdatedb --localpaths=$search_path --output=$locate_db
