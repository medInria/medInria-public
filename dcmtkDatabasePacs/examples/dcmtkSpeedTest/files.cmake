
SET (MED_DCMTK_SOURCES
		../../medDcmtk/dcmtkLogger.h
		../../medDcmtk/dcmtkLogger.cpp
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
		../../medDcmtk/dcmtkResultDataset.h
		../../medDcmtk/dcmtkResultDataset.cpp
		../../medDcmtk/dcmtkKey.h
		../../medDcmtk/dcmtkKey.cpp		
		../../medDcmtk/dcmtkNode.h
		../../medDcmtk/dcmtkNode.cpp
		
		../../medDcmtk/dcmtkBaseScp.h
		../../medDcmtk/dcmtkBaseScp.cpp
		../../medDcmtk/dcmtkStoreScp.h
		../../medDcmtk/dcmtkStoreScp.cpp	
		
	)
	
SET (MED_LOGGER_SOURCES
		../../medLogger/BaseLogger.h
		../../medLogger/BaseLogger.cpp
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
		../../medDcmtk/dcmtkStoreScp.h
		../../medDcmtk/dcmtkFindScu.h
		../../medDcmtk/dcmtkBaseScu.h
	)
	
SET (MED_GENERAL_SOURCES
	    main.cpp
		SpeedTest.h
		SpeedTest.cpp
	)