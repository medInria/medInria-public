
# sample
IF (WIN32)
   SET(GUI_EXECUTABLE WIN32)
ENDIF(WIN32)
  
IF(APPLE)
   SET(GUI_EXECUTABLE MACOSX_BUNDLE)
ENDIF(APPLE)

ADD_EXECUTABLE(fpbtest ${GUI_EXECUTABLE}
foldpanelbartest.cpp
foldtestpanel.cpp
)

TARGET_LINK_LIBRARIES(fpbtest
${wxWidgets_LIBRARIES}
wxAddOn
)

ADD_EXECUTABLE(wxFlatNotebookTest ${GUI_EXECUTABLE}
wxFlatNotebookTest.cpp
resources.cpp
Frame.cpp
)

TARGET_LINK_LIBRARIES(wxFlatNotebookTest
${wxWidgets_LIBRARIES}
wxAddOn
)

INSTALL_TARGETS(/bin fpbtest wxFlatNotebookTest)
