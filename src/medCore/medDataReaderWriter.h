#ifndef MED_DATA_READER_WRITER_H
#define MED_DATA_READER_WRITER_H

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>

namespace medInria {

    struct DataReaderWriter {

        typedef dtkSmartPointer<dtkAbstractDataReader> Reader;
        typedef dtkSmartPointer<dtkAbstractDataWriter> Writer;
        typedef dtkSmartPointer<dtkAbstractData>       Data;

        static Reader reader(const QString& path);
        static Writer writer(const QString& path,const dtkAbstractData* data);

        static Data read(const QString& path);
        static bool write(const QString& path,dtkAbstractData* data);
    };
}

#endif  //  ! MED_DATA_READER_WRITER_H
