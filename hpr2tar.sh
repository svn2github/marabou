tar --exclude hpr/src/G_*.cxx -czf hpr.tgz hpr/src/*.cxx hpr/inc/*.h
echo "List of hpr.tgz -------"
tar tzf hpr.tgz
