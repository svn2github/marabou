#!/bin/sh

echo "Starting ca_test ..."

rm -f *_s.tcl
../ca/server/daq_tcl_param_cas 2>cas_test_protocol.txt &

echo $! >> cas.pid

