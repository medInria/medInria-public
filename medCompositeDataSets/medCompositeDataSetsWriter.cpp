#include "medCompositeDataSetsWriter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <dtkZip/dtkZip.h>
#include <dtkZip/dtkZipFile.h>

#include <medCompositeDataSetsBase.h>

bool medCompositeDataSetsWriter::canWrite(const QString& path) {
    
    if (!this->data())
        return false;

    dtkAbstractData *dtkdata = this->data();

    writer = MedInria::medCompositeDataSetsBase::known(dtkdata->description().toAscii().data(),-1);
    return writer!=0;
}

bool medCompositeDataSetsWriter::write(const QString& path) {
    QFile zipFile(path);
    dtkZip zip(&zipFile);

    if (!zip.open(dtkZip::mdCreate))
        return false;
    zip.setComment("Composite data set");

    //  Write the data in a temporary directory.

    QString tmpdir = QString(mkdtemp((QDir::tempPath()+"/"+"medcdsXXXXXXX").toAscii().data()));
    writer->write_data(tmpdir,this->data());

    //  Zip the temporary directory.

    QFileInfoList files = QDir(tmpdir).entryInfoList();

    dtkZipFile outFile(&zip);

    foreach(QFileInfo file,files) {
        QFile inFile(file.fileName());

        if(!inFile.open(QIODevice::ReadOnly)) {
            qWarning("medCompositeDataSets: inFile.open(): %s", inFile.errorString().toLocal8Bit().constData());
            return false;
        }

        if(!outFile.open(QIODevice::WriteOnly, dtkZipNewInfo(inFile.fileName(), inFile.fileName()))) {
            qWarning("medCompositeDataSets: outFile.open(): %d", outFile.getZipError());
            return false;
        }

        qint64 len = file.size();
        qint64 pos = 0;

        char c;

        while (inFile.getChar(&c) && outFile.putChar(c)) {

            char buf[4096];
            qint64 l = inFile.read(buf,4096);

            if (l < 0) {
                qWarning("read(): %s",inFile.errorString().toUtf8().constData());
                break;
            }

            if (l==0)
                break;

            if (outFile.write(buf,l)!=l) {
                qWarning("write(): %d",outFile.getZipError());
                break;
            }

            pos += l;

            if (pos%1048576==0)
                qDebug("%.1f",(float) pos/len*100.0f);
        }

        if(outFile.getZipError()!=UNZ_OK) {
            qWarning("medCompositeDataSets: outFile.putChar(): %d", outFile.getZipError());
            return false;
        }

        outFile.close();

        if(outFile.getZipError()!=UNZ_OK) {
            qWarning("medCompositeDataSets: outFile.close(): %d", outFile.getZipError());
            return false;
        }

        inFile.close();
    }

    zip.close();

    if(zip.getZipError()!=0) {
        qWarning("medCompositeDataSets: zip.close(): %d", zip.getZipError());
        return false;
    }

    return true;
}
