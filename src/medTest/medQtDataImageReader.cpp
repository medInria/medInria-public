/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medQtDataImageReader.h"

#include "medQtDataImage.h"

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <QImage>
#include <QImageReader>

//static members

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////
dtkAbstractDataReader *createMedQtDataImageReader(void)
{
    return new medQtDataImageReader;
}

medQtDataImageReader::medQtDataImageReader()
{
    m_lastSuccess = false;
}

medQtDataImageReader::~medQtDataImageReader()
{
}

bool medQtDataImageReader::registered(void)
{
    return dtkAbstractDataFactory::instance()->registerDataReaderType(
        medQtDataImageReader::s_description(),
        medQtDataImageReader::s_handled(),
        createMedQtDataImageReader);
}

QString medQtDataImageReader::identifier() const {
    return s_description();
}

QString medQtDataImageReader::description() const {
    return s_description();
}

QStringList medQtDataImageReader::handled() const
{
    return s_handled();
}

bool medQtDataImageReader::canRead( const QString& path )
{
    QImageReader reader(path);
    return reader.canRead();
}

bool medQtDataImageReader::canRead( const QStringList& paths )
{
    foreach( const QString path, paths ) {
        if (! canRead(path) )
            return false;
    }
    return true;
}

bool medQtDataImageReader::readInformation( const QString& path )
{
    return this->read( path );
}

bool medQtDataImageReader::readInformation( const QStringList& paths )
{
    return this->read(paths);
}


void medQtDataImageReader::reset()
{
    m_reader.reset(NULL);
    this->setData(NULL);
}

void medQtDataImageReader::setMetaDataFromImageReader()
{
    medAbstractData * dtkdata = dynamic_cast<medAbstractData*>(this->data());
    if ( !dtkdata || m_reader.isNull() )
        return;

    const QStringList keys = m_reader->textKeys();
    foreach( const QString key, keys) {
        dtkdata->setMetaData( key, m_reader->text(key) );
    }
}

bool medQtDataImageReader::read( const QStringList& paths )
{
    if ( this->m_lastPath == paths ) {
        return m_lastSuccess;
    }

    this->m_lastPath = paths;

    int numRead(0);
    this->setProgress(0);
    foreach( const QString path, paths ) {
        numRead += this->readStartingFrom(path, numRead);
        double progress = static_cast<double>(numRead)/static_cast<double>(paths.size()) * 100.;
        this->setProgress(static_cast<int>(progress));
    }
    this->setProgress(100);
    m_lastSuccess = (numRead != 0);
    return m_lastSuccess;
}

bool medQtDataImageReader::read( const QString& path )
{
    if ( this->m_lastPath.size() == 1 && this->m_lastPath[0] == path ) {
        return m_lastSuccess;
    }

    this->setProgress(0);
    this->m_lastPath.clear();
    this->m_lastPath << path;

    int numRead = this->readStartingFrom(path, 0);
    this->setProgress(100);

    m_lastSuccess = (numRead != 0);
    return m_lastSuccess;
}

int medQtDataImageReader::readStartingFrom( const QString& path, int iStart )
{
    if (iStart == 0 ) {
        this->reset();
    }

    m_reader.reset( new QImageReader( path ) );
    if ( !m_reader->canRead() )
        return 0;


    this->setData( dtkAbstractDataFactory::instance()->createSmartPointer(medQtDataImage::s_identifier()) );

    this->setMetaDataFromImageReader();

    QImage nextImage = m_reader->read();
    if ( nextImage.isNull() ) {
        this->setData(NULL);
        return 0;
    }

    medAbstractData * dtkData = dynamic_cast<medAbstractData*>(this->data());

    int numRead(0);
    if ( m_reader->supportsAnimation() ) {
        int imageCount = m_reader->imageCount();
        for( int i(0); i<imageCount; ++i ) {
            m_reader->jumpToImage(i);
            nextImage = m_reader->read();
            dtkData->setData(&(nextImage), i + iStart);
        }
        numRead = imageCount;
    } else {
        dtkData->setData(&(nextImage), iStart);
        numRead = 1;
    }
    return numRead;
}

void medQtDataImageReader::setProgress( int value )
{
  emit progressed (value);
}

QString medQtDataImageReader::s_description()
{
    const QString description = "medQtDataImageReader";
    return description;
}

QStringList medQtDataImageReader::s_handled()
{
    const QStringList handled = QStringList() << medQtDataImage::s_identifier();
    return handled;
}
















