/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medTestExport.h"

#include <dtkCore/dtkAbstractDataReader.h>

class dtkAbstractDataImage;

/** A concrete implementation of a dtkAbstractDataReader for use in testing. */
class MEDTEST_EXPORT medQtDataImageReader : public dtkAbstractDataReader {
    Q_OBJECT;

public: //functions
    medQtDataImageReader();
    virtual ~medQtDataImageReader();

    // Implement dtkAbstractDataReader
    virtual QString identifier()  const;
    virtual QString description() const;
    virtual QStringList handled() const;

    static QString s_description();
    static QStringList s_handled();

    static bool registered();

public slots:

    virtual bool canRead (const QString& path);
    virtual bool canRead (const QStringList& paths);

    virtual bool readInformation (const QString& path);
    virtual bool readInformation (const QStringList& paths);

    virtual bool read (const QString& path);
    virtual bool read (const QStringList& paths);

    virtual void setProgress (int value);

protected:
    //! Clear data.
    void reset();

    //! Set metadata for image from reader.
    void setMetaDataFromImageReader();

    //! read images, starting from index i. Return number read.
    int readStartingFrom (const QString& path, int iStart);

public: // data members

private:
    QScopedPointer<QImageReader> m_reader;
    QStringList m_lastPath;
    bool m_lastSuccess;
};



