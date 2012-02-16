#include <medDataReaderWriter.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <QFileInfo>

namespace medInria {

    DataReaderWriter::Reader DataReaderWriter::reader(const QString& path) {
        QList<QString> readers = dtkAbstractDataFactory::instance()->readers();
            
        if (readers.size()==0) {
#if 0
            emit showError(this,tr("No reader plugin"),5000);
            emit failure(this);
#endif
            return Reader();
        }

        const QFileInfo& fi(path);
        const QString&   filename = fi.filePath();

        //  Store the last reader.

        static Reader dreader;

        if (!dreader.isNull() && dreader->canRead(filename))
            return dreader;

        for (int i=0;i<readers.size();++i) {
            dreader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
            if (dreader->canRead(filename)) {
                dreader->enableDeferredDeletion(false);
                return dreader;
            }
        }
        return dreader = Reader();
    }

    DataReaderWriter::Writer DataReaderWriter::writer(const QString& path,const dtkAbstractData* data) {

        if (!data)
            return Writer();

        QList<QString> writers = dtkAbstractDataFactory::instance()->writers();

        static Writer dwriter;

        if (!dwriter.isNull() && dwriter->handled().contains(data->identifier()) && dwriter->canWrite(path))
            return dwriter;

        for (int i=0;i<writers.size();++i) {
            dwriter = dtkAbstractDataFactory::instance()->writerSmartPointer(writers[i]);
            if (dwriter->handled().contains(data->identifier()) && dwriter->canWrite(path)) {
                dwriter->enableDeferredDeletion(false);
                return dwriter;
            }
        }
        return dwriter = Writer();
    }

    DataReaderWriter::Data DataReaderWriter::read(const QString& path) {
        Reader dreader = reader(path);
        if (!dreader.isNull()) {
            dreader->read(path);
            return dreader->data();
        }
    }

    bool DataReaderWriter::write(const QString& path,dtkAbstractData* data) {
        Writer dwriter = writer(path,data);
        if (!dwriter.isNull()) {
            dwriter->setData(data);
            dwriter->write(path);
            return true;
        }
        return false;
    }  
}
