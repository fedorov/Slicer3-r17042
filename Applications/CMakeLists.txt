PROJECT(Applications)

IF(Slicer3_BUILD_CLI)
  ADD_SUBDIRECTORY(CLI)
ENDIF(Slicer3_BUILD_CLI)

IF (Slicer3_USE_KWWIDGETS)
  ADD_SUBDIRECTORY(GUI)
ENDIF (Slicer3_USE_KWWIDGETS)

IF(Slicer3_USE_QT)
  ADD_SUBDIRECTORY(SlicerQT)
ENDIF(Slicer3_USE_QT)

IF(Slicer3_BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(Slicer3_BUILD_TESTING)
