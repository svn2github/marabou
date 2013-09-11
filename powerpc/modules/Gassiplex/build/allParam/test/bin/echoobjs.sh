#!/bin/sh

echo "all : $(sed s/[\.]c/.o/g << EOF
$(echo *.c)
EOF
)"

