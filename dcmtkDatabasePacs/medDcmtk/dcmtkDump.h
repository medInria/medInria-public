/**
* This software was developed using DCMTK from OFFIS e.V.
*
* Kuratorium OFFIS e.V.
* Healthcare Information and Communication Systems
* Escherweg 2
* D-26121 Oldenburg, Germany
*/


#ifndef DCMTKDUMP_H
#define DCMTKDUMP_H

// inc
#include "dcmtkBase.h"

/**
* @class dcmtkDump
* @author Michael Knopke
* @brief List the contents of a DICOM file. Converted from dcmDump
*/
class dcmtkDump : public dcmtkBase
{
public:

    dcmtkDump();
    ~dcmtkDump();


    /**
    * doDump - Dump all DICOM Attributes found into a string
    * @params: const char * inFilename Filename and Path to DICOM File
    * @params: std::string & output Output of parsing
    * @return   int 0 if success 1 for failure
    */
    int doDump(const char* inFilename, std::string &output);

    /**
    * setPixelDirectory - If you provide a valid directory, the pixeldata contained in the file will be extracted and put here
    * @params: const char * dir Directory to put Pixeldata into
    * @return   void
    */
    void setPixelDirectory(const char* dir);

protected:

    void printResult(STD_NAMESPACE ostream &out,
                    DcmStack &stack,
                    size_t printFlags,
                    const char*pixelFileName = NULL,
                    size_t *pixelCounter = NULL);

    int dumpFile(STD_NAMESPACE ostream &out,
                const char *ifname,
                const E_FileReadMode readMode,
                const E_TransferSyntax xfer,
                const size_t printFlags,
                const OFBool loadIntoMemory,
                const OFBool stopOnErrors,
                const char *pixelDirectory);

    DcmTagKey parseTagKey(const char *tagName);

    OFBool addPrintTagName(const char *tagName);

private:

    OFBool printFilename;
    OFBool printFileSearch;
    OFBool printAllInstances;
    OFBool prependSequenceHierarchy;
    int printTagCount;
    const char *printTagNames[1024];
    const DcmTagKey *printTagKeys[1024];
    OFCmdUnsignedInt maxReadLength;
    size_t fileCounter;
    OFBool loadIntoMemory;
    size_t printFlags;
    E_FileReadMode readMode;
    E_TransferSyntax xfer;
    OFBool stopOnErrors;
    const char *pixelDirectory;


};


#endif // DCMTKDUMP_H