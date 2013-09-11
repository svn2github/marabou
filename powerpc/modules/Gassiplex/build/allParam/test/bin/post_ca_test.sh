#!/bin/sh

kill $(cat cas.pid)
rm -f cas.pid

echo "Cleaned up after ca_test."

