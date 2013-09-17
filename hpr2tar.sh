tar  --exclude helpbrowser/src/G_*.cxx --exclude hpr/src/G_*.cxx   \
--exclude fitcal/src/G_*.cxx  --exclude  gredit/src/G_*.cxx  --exclude gutils/src/G_*.cxx \
 -czf hpr.tgz Makefile hpr/Module.mk hpr/src/*.cxx hpr/inc/*.h \
fitcal/Module.mk fitcal/src/*.cxx fitcal/inc/*.h \
gredit/Module.mk gredit/src/*.cxx  gredit/inc/*.h  utils/Module.mk utils/src/*.cxx utils/inc/*.h \
gutils/Module.mk gutils/src/*.cxx gutils/inc/*.h icons/*.xpm \
macros/*.C doc/hpr/* helpbrowser/src/*.cxx helpbrowser/inc/*.h
echo "List of hpr.tgz ------"
tar -tzf hpr.tgz
