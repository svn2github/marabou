#!/bin/sh

sqlplus daq_oper/daqall@db-hades.gsi.de @bin/cleanup.sql
echo "Cleaned up after ora_test."

