/*
 * medQtDataImageReader.h
 *
 *  Created on: 4 juil. 2011 10:47:18
 *      Author: John Stark
 */

#ifndef MEDQTDATAIMAGEREADER_H_
#define MEDQTDATAIMAGEREADER_H_

#include "medTestCommonExport.h"

#include <dtkCore/dtkAbstractDataReader.h>

class dtkAbstractDataImage;

/** A concrete implementation of a dtkAbstractDataReader for use in testing. */
class MEDTESTCOMMON_EXPORT medQtDataImageReader : public dtkAbstractDataReader {
    Q_OBJECT;

public: //functions
    medQtDataImageReader();
    virtual ~medQtDataImageReader();

    // Implement dtkAbstractDataReader
    virtual QString description(void) const;
    virtual QStringList handled(void) const;

    static QString s_description();
    static QStringList s_handled();

    static bool registered();

public slots:

    virtual bool canRead (const QString& path);
    virtual bool canRead (const QStringList& paths);

    virtual void readInformation (const QString& path);
    virtual void readInformation (const QStringList& paths);

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

#endif /* MEDQTDATAIMAGEREADER_H_ */

