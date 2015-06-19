/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medTestExport.h>

#include <dtkCoreSupport/dtkAbstractDataWriter.h>

/** A concrete implementation of a dtkAbstractDataWriter for use in testing. */
class MEDTEST_EXPORT medQtDataImageWriter : public dtkAbstractDataWriter
{
    Q_OBJECT;
public:
    medQtDataImageWriter();
    virtual ~medQtDataImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;
    virtual QStringList handled() const;

    static QString s_description();
    static QString s_identifier();
    static QStringList s_handled();

    virtual QStringList supportedFileExtensions() const;

    static bool registered();

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

protected:
public:

    bool writeOrTest    (const QString& path, bool dryRun = true);

    struct FormatInfo { 
        FormatInfo( QString fileExtension_, QByteArray formatName_, int priority_ ) : 
            fileExtension(fileExtension_) , formatName(formatName_), priority(priority_) {  }
        QString fileExtension;
        QByteArray formatName;
        int priority; };

    typedef QList<FormatInfo> FormatInfoList;
    QList<FormatInfo> m_supportedExtensionList;

protected:

};



