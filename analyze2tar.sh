tar --exclude analyze/src/G_*.cxx -czf analyze.tgz analyze/src/*.cxx analyze/inc/*.h
echo "List of analyze.tgz -------"
tar tzf analyze.tgz
