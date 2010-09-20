
SET (MED_DCMTK_SOURCES
		../../medDcmtk/dcmtkLogger.h
		../../medDcmtk/dcmtkLogger.cpp
		../../medDcmtk/dcmtkFindDataset.h
		../../medDcmtk/dcmtkFindDataset.cpp
		../../medDcmtk/dcmtkBase.h
		../../medDcmtk/dcmtkBase.cpp
		../../medDcmtk/dcmtkBaseScu.h
		../../medDcmtk/dcmtkBaseScu.cpp
		../../medDcmtk/dcmtkEchoScu.h
		../../medDcmtk/dcmtkEchoScu.cpp
		../../medDcmtk/dcmtkFindScu.h
		../../medDcmtk/dcmtkFindScu.cpp
		../../medDcmtk/dcmtkFindCallback.h
		../../medDcmtk/dcmtkFindCallback.cpp
		../../medDcmtk/dcmtkMoveScu.h
		../../medDcmtk/dcmtkMoveScu.cpp
		../../medDcmtk/dcmtkStoreScu.h
		../../medDcmtk/dcmtkStoreScu.cpp
		../../medDcmtk/dcmtkContainer.h
		../../medDcmtk/dcmtkContainer.txx
		../../medDcmtk/dcmtkNodeContainer.h
		
		../../medDcmtk/dcmtkBaseScp.h
		../../medDcmtk/dcmtkBaseScp.cpp
		../../medDcmtk/dcmtkStoreScp.h
		../../medDcmtk/dcmtkStoreScp.cpp	
		
	)
	
SET (MED_LOGGER_SOURCES
		../../medLogger/Logger.h
		../../medLogger/Logger.cpp
		../../medLogger/LoggerConsoleOutput.h
		../../medLogger/LoggerConsoleOutput.cpp
		../../medLogger/LoggerFileOutput.h
		../../medLogger/LoggerFileOutput.cpp
		../../medLogger/LoggerWidget.h
		../../medLogger/LoggerWidget.cpp
		../../medLogger/LoggerWidgetOutput.h
		../../medLogger/LoggerWidgetOutput.cpp		
		../../medLogger/LoggerLogStream.h
		../../medLogger/LoggerLogStream.cpp
		../../medLogger/LoggerLogLevel.h
		../../medLogger/LoggerOutput.h
		../../medLogger/LoggerOutput.cpp
		../../medLogger/LoggerMessageEvent.h
	)
	
SET (OTHER_MOC_FILES
		../../medLogger/LoggerWidget.h
	)
	
SET (MED_GENERAL_SOURCES
	    ServerThread.h
		ServerThread.cpp
		SendThread.h
		SendThread.cpp
	)