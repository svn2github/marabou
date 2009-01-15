TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt

INCLUDEPATH += inc $(ROOTSYS)/include
HEADERS	+= inc/QEnv.h
SOURCES	+= src/QEnv.cxx src/qenvExample.cxx 
TARGET = qenvExample

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


