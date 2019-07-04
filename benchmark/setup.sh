#!/bin/bash
set -e
mid_folder="${HOME}/working/3p/src/"
big_folder="/"

# Create an indexed database for fast-locate
fast_locate_db_mid=".database_mid"
fast_locate_db_big=".database_big"

# Generate the test data using mid-size folder
rm -f $fast_locate_db_mid
rm -f $fast_locate_db_big
fast-updatedb $mid_folder -d "$fast_locate_db_mid"
fast-updatedb $big_folder -d "$fast_locate_db_big"

# Create an indexed database for GNU locate
locate_db_mid="locate_db_mid"
locate_db_big="locate_db_big"

rm -f $locate_db_mid
rm -f $locate_db_big

osType=$(uname)
case "$osType" in
    "Darwin")
        {
            gupdatedb --localpaths=$mid_folder --output=$locate_db_mid
            gupdatedb --localpaths=$big_folder --output=$locate_db_big
        } ;;
    "Linux")
        {
            updatedb -l 0 -U $mid_folder -o $locate_db_mid
            updatedb -l 0 -U $big_folder -o $locate_db_big
        } ;;
    *)
        {
            echo "Unsupported OS, exiting"
            exit
        } ;;
esac
