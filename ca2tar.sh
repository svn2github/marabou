tar --exclude c_analyze/src/G_*.cxx -czf c_analyze.tgz c_analyze/src/*.cxx c_analyze/inc/*.h
echo "List of c_analyze.tgz -------"
tar tzf c_analyze.tgz
