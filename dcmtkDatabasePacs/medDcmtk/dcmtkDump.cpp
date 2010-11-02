#include "dcmtkDump.h"

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"

#include "dcmtkLogger.h"

#include <sstream>

//---------------------------------------------------------------------------------------------

dcmtkDump::dcmtkDump()
{
    printFilename = OFFalse;
    printFileSearch = OFFalse;
    printAllInstances = OFTrue;
    prependSequenceHierarchy = OFFalse;
    printTagCount = 0;
    maxReadLength = 4096; // default is 4 KB
    fileCounter = 0;

    loadIntoMemory = OFTrue;
    printFlags = DCMTypes::PF_shortenLongTagValues;
    readMode = ERM_autoDetect;
    xfer = EXS_Unknown;
    stopOnErrors = OFFalse;
    pixelDirectory = NULL;
}

//---------------------------------------------------------------------------------------------

dcmtkDump::~dcmtkDump()
{

}

//---------------------------------------------------------------------------------------------

int dcmtkDump::doDump(const char* inFilename, std::string &output)
{

    /* make sure the pixel data directory exists and is writable */
    if (pixelDirectory != NULL)
    {
        if (!OFStandard::dirExists(pixelDirectory))
        {
            dcmtkLogger::warningStream() << "directory specified for --write-pixel does not exist, ignoring this option";
            pixelDirectory = NULL;
        }
        else if (!OFStandard::isWriteable(pixelDirectory))
        {
            dcmtkLogger::warningStream() << "directory specified for --write-pixel is not writeable, ignoring this option";
            pixelDirectory = NULL;
        }
    }

    std::stringstream outstream;
    int err = dumpFile(outstream, inFilename, readMode, xfer, printFlags, loadIntoMemory, stopOnErrors, pixelDirectory);
    output = outstream.str();
    return err;
}

//---------------------------------------------------------------------------------------------

void dcmtkDump::printResult(STD_NAMESPACE ostream &out,
                            DcmStack &stack,
                            size_t printFlags,
                            const char*pixelFileName,
                            size_t *pixelCounter)
{
    unsigned long n = stack.card();
    if (n == 0) {
        return;
    }

    if (prependSequenceHierarchy) {
        /* print the path leading up to the top stack elem */
        for (unsigned long i = n - 1; i >= 1; i--) {
            DcmObject *dobj = stack.elem(i);
            /* do not print if a DCM_Item as this is not
            * very helpful to distinguish instances.
            */
            if (dobj != NULL && dobj->getTag().getXTag() != DCM_Item) {
                out << dobj->getTag() << ".";
            }
        }
    }

    /* print the tag and its value */
    DcmObject *dobj = stack.top();
    dobj->print(out, printFlags, 1 /*level*/, pixelFileName, pixelCounter);
}

//---------------------------------------------------------------------------------------------

int dcmtkDump::dumpFile(STD_NAMESPACE ostream &out,
                        const char *ifname,
                        const E_FileReadMode readMode,
                        const E_TransferSyntax xfer,
                        const size_t printFlags,
                        const OFBool loadIntoMemory,
                        const OFBool stopOnErrors,
                        const char *pixelDirectory)
{
    int result = 0;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        dcmtkLogger::errorStream() << ": invalid filename: <empty string>";
        return 1;
    }

    DcmFileFormat dfile;
    DcmObject *dset = &dfile;
    if (readMode == ERM_dataset) dset = dfile.getDataset();
    OFCondition cond = dfile.loadFile(ifname, xfer, EGL_noChange, maxReadLength, readMode);
    if (cond.bad())
    {
        dcmtkLogger::errorStream() << ": " << cond.text()
            << ": reading file: "<< ifname;
        result = 1;
        if (stopOnErrors) return result;
    }

    if (loadIntoMemory) dfile.loadAllDataIntoMemory();

    size_t pixelCounter = 0;
    const char *pixelFileName = NULL;
    OFString pixelFilenameStr;
    if (pixelDirectory != NULL)
    {
        /* create filename for pixel data */
        OFString fileName;
        OFStandard::getFilenameFromPath(fileName, ifname);
        OFStandard::combineDirAndFilename(pixelFilenameStr, pixelDirectory, fileName);
        pixelFileName = pixelFilenameStr.c_str();
    }

    /* dump complete file content */
    if (printTagCount == 0)
    {
        dset->print(out, printFlags, 0 /*level*/, pixelFileName, &pixelCounter);
    } else {
        OFBool firstTag = OFTrue;
        /* only print specified tags */
        for (int i = 0; i < printTagCount; i++)
        {
            unsigned int group = 0xffff;
            unsigned int elem = 0xffff;
            DcmTagKey searchKey;
            const char *tagName = printTagNames[i];
            if (printTagKeys[i])
                searchKey = *printTagKeys[i];
            else if (sscanf(tagName, "%x,%x", &group, &elem) == 2)
                searchKey.set(group, elem);
            else {
                dcmtkLogger::errorStream() << "Internal ERROR in File " << __FILE__ << ", Line "
                    << __LINE__ << " -- Named tag inconsistency";
                abort();
            }

            DcmStack stack;
            if (dset->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal)
            {
                if (firstTag)
                {
                    if (!printFilename)
                    {
                        /* a newline separates two consecutive "dumps" */
                        if (++fileCounter > 1)
                            COUT << OFendl;
                    }
                    /* print header with filename */
                    if (printFileSearch)
                        dcmtkLogger::infoStream() << " (" << fileCounter << "): " << ifname;
                    firstTag = OFFalse;
                }
                printResult(out, stack, printFlags, pixelFileName, &pixelCounter);
                if (printAllInstances)
                {
                    while (dset->search(searchKey, stack, ESM_afterStackTop, OFTrue) == EC_Normal)
                        printResult(out, stack, printFlags, pixelFileName, &pixelCounter);
                }
            }
        }
    }
    return result;
}

//---------------------------------------------------------------------------------------------

DcmTagKey dcmtkDump::parseTagKey(const char *tagName)
{
    unsigned int group = 0xffff;
    unsigned int elem = 0xffff;
    if (sscanf(tagName, "%x,%x", &group, &elem) != 2)
    {
        DcmTagKey tagKey;
        /* it is a name */
        const DcmDataDictionary &globalDataDict = dcmDataDict.rdlock();
        const DcmDictEntry *dicent = globalDataDict.findEntry(tagName);
        if (dicent == NULL) {
            dcmtkLogger::warningStream() << "unrecognized tag name: '" << tagName << "'";
            tagKey = DCM_UndefinedTagKey;
        } else {
            tagKey = dicent->getKey();
        }
        dcmDataDict.unlock();
        return tagKey;
    } else     /* tag name has format "gggg,eeee" */
    {
        return DcmTagKey(group,elem);
    }
}

//---------------------------------------------------------------------------------------------

OFBool dcmtkDump::addPrintTagName(const char *tagName)
{
    if (printTagCount >= 1024) {
        dcmtkLogger::warningStream() << "too many print tag options (max: " << 1024 << ")";
        return OFFalse;
    }

    unsigned int group = 0xffff;
    unsigned int elem = 0xffff;
    if (sscanf(tagName, "%x,%x", &group, &elem) != 2)
    {
        /* it is a name */
        const DcmDataDictionary &globalDataDict = dcmDataDict.rdlock();
        const DcmDictEntry *dicent = globalDataDict.findEntry(tagName);
        if (dicent == NULL) {
            dcmtkLogger::warningStream() << "unrecognized tag name: '" << tagName << "'";
            dcmDataDict.unlock();
            return OFFalse;
        } else {
            /* note for later */
            printTagKeys[printTagCount] = new DcmTagKey(dicent->getKey());
        }
        dcmDataDict.unlock();
    } else {
        /* tag name has format xxxx,xxxx */
        /* do not lookup in dictionary, tag could be private */
        printTagKeys[printTagCount] = NULL;
    }

    printTagNames[printTagCount] = strcpy(OFstatic_cast(char*, malloc(strlen(tagName)+1)), tagName);
    printTagCount++;
    return OFTrue;
}

//---------------------------------------------------------------------------------------------

void dcmtkDump::setPixelDirectory( const char* dir )
{
    pixelDirectory = dir;
}

//---------------------------------------------------------------------------------------------
