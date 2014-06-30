/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQtDataImageWriter.h>
#include <medQtDataImage.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkLog/dtkLog.h>

namespace {
    // Used to sort formats in order of priority, highest first.
    class CompareReversePriority {
    public:
        bool operator() ( const medQtDataImageWriter::FormatInfoList::const_iterator & r,
            const medQtDataImageWriter::FormatInfoList::const_iterator & l ) const
        { if ( r->priority != l->priority )
            return r->priority > l->priority;
        else
            return r->fileExtension > l->fileExtension;
        }
    };
}

dtkAbstractDataWriter * createMedQtDataImageWriter()
{
    return new medQtDataImageWriter;
}

medQtDataImageWriter::medQtDataImageWriter()
{
    // Extensions with higher priority will appear first in supportedExtension list.
    // Generally we prefer lossless, and portable formats.
    m_supportedExtensionList.push_back(FormatInfo("png",   QByteArray::fromRawData("PNG",3), 100) );
    m_supportedExtensionList.push_back(FormatInfo("jpg",   QByteArray::fromRawData("JPG",3), 50) );
    m_supportedExtensionList.push_back(FormatInfo("jpeg",  QByteArray::fromRawData("JPEG",4), 49) );
    m_supportedExtensionList.push_back(FormatInfo("bmp",   QByteArray::fromRawData("BMP",3), 60) );
    m_supportedExtensionList.push_back(FormatInfo("ppm",   QByteArray::fromRawData("PPM",3), 75) );
    m_supportedExtensionList.push_back(FormatInfo("xbm",   QByteArray::fromRawData("XBM",3), 72) );
    m_supportedExtensionList.push_back(FormatInfo("tiff",  QByteArray::fromRawData("TIFF",4), 70) );
    m_supportedExtensionList.push_back(FormatInfo("tif",   QByteArray::fromRawData("TIFF",4), 75) );
}

medQtDataImageWriter::~medQtDataImageWriter()
{

}

QString medQtDataImageWriter::identifier() const {
    return s_identifier();
}

QString medQtDataImageWriter::description() const {
    return s_description();
}

QStringList medQtDataImageWriter::handled() const
{
    return s_handled();
}

QStringList medQtDataImageWriter::supportedFileExtensions() const
{
    QStringList extensions;
#if QT_VERSION > 0x0406FF
    extensions.reserve( m_supportedExtensionList.size());
#endif
    QList< FormatInfoList::const_iterator > sortedFormats;
    for(  FormatInfoList::const_iterator it( m_supportedExtensionList.begin() ); it != m_supportedExtensionList.end(); ++it ) {
        sortedFormats.push_back(it);
    }
    CompareReversePriority compareReversePriority;
    qSort( sortedFormats.begin(), sortedFormats.end(), compareReversePriority );

    foreach( FormatInfoList::const_iterator it, sortedFormats ) {
        extensions << ( "." + it->fileExtension );
    }
    return extensions;
}

bool medQtDataImageWriter::registered()
{
    return medAbstractDataFactory::instance()->registerDataWriterType( s_description(), s_handled(), createMedQtDataImageWriter );
}

bool medQtDataImageWriter::write( const QString& path )
{
    return this->writeOrTest(path, false);
}

bool medQtDataImageWriter::canWrite( const QString& path )
{
    QFileInfo fi ( path );
    QString suffix = fi.suffix();

    QByteArray fmtString;
    const QString lowerSuffix = suffix.toLower();
    FormatInfoList::const_iterator formatToUse = m_supportedExtensionList.begin();
    for ( ; formatToUse != m_supportedExtensionList.end() ; ++formatToUse ){
        if (formatToUse->fileExtension == suffix )
            break;
    }
    if ( formatToUse != m_supportedExtensionList.end() ) {
        fmtString = formatToUse->formatName;
    } else {
        dtkDebug() << "Filename does not have a supported extension";
        return false;
    }

    QScopedPointer<QImageWriter> writer(new QImageWriter( path ) );
    if ( fmtString.size() ) {
        writer->setFormat( fmtString );
    }
    
    return writer->canWrite();
}

bool medQtDataImageWriter::writeOrTest( const QString& path, bool dryRun /*= true*/ )
{
    medAbstractData * medData = dynamic_cast<medAbstractData*>(this->data());

    if ( !medData )
        return false;

    if (medData->identifier() != medQtDataImage::staticIdentifier() ) {
        return false;
    }

    QFileInfo fi ( path );
    QString suffix = fi.suffix();

    QByteArray fmtString;
    const QString lowerSuffix = suffix.toLower();
    FormatInfoList::const_iterator formatToUse = m_supportedExtensionList.begin();
    for ( ; formatToUse != m_supportedExtensionList.end() ; ++formatToUse ){
        if (formatToUse->fileExtension == suffix )
            break;
    }
    if ( formatToUse != m_supportedExtensionList.end() ) {
        fmtString = formatToUse->formatName;
    } else {
        dtkDebug() << "Filename does not have a supported extension";
        return false;
    }

    QScopedPointer<QImageWriter> writer(new QImageWriter( path ) );
    if ( fmtString.size() ) {
        writer->setFormat( fmtString );
    }

    medAbstractImageData * medDataIm = dynamic_cast< medAbstractImageData *>( medData );

    // Set metadata
    const QStringList keys = medData->metaDataList();
    foreach( const QString key, keys) {
        writer->setText( key, medData->metadata(key) );
    }

    const int numImage = medDataIm->zDimension();
    int numWritten = 0;
    if ( numImage > 1) {
        QString filetemplate = path;
        filetemplate.resize( path.size() - suffix.size() );

        for ( int i(0); i<numImage; ++i ) {
            QString filename = filetemplate + QString::number(i) + suffix;
            writer->setFileName(filename);
            QImage * image = reinterpret_cast<QImage *>(medDataIm->data( i ));
            bool success =
                dryRun ? writer->canWrite() : writer->write( *image );
            if ( success )
                ++numWritten;
        }
    } else if ( numImage == 1 ) {
        QImage * image = reinterpret_cast<QImage *>(medDataIm->data());
        bool success = dryRun ? writer->canWrite() : writer->write( *image );
        if ( success )
            ++numWritten;
    } else {

    }
    return numWritten && (numWritten == numImage);
}

QStringList medQtDataImageWriter::s_handled()
{
    static const QStringList handled = QStringList() << medQtDataImage::staticIdentifier();
    return handled;
}

QString medQtDataImageWriter::s_description()
{
    static const QString description = "Dummy writer for the medQtDataImage type";
    return description;
}

QString medQtDataImageWriter::s_identifier()
{
    static const QString identifier = "medQtDataImageWriter";
    return identifier;
}

