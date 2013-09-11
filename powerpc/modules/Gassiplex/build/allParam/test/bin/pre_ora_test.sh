#!/bin/sh

echo "Starting ora_test ..."

rm -f *_s.tcl

cp tcl/i_store.tcl i_store_s.tcl
cp tcl/i_store_existing.tcl i_store_existing_s.tcl
cp tcl/i_store_multiple.tcl i_store_multiple_s.tcl
cp tcl/s_store.tcl s_store_s.tcl
cp tcl/s_store_existing.tcl s_store_existing_s.tcl
cp tcl/s_store_multiple.tcl s_store_multiple_s.tcl
cp tcl/s_store_whitespace.tcl s_store_whitespace_s.tcl

