/*
 * medQtDataImageWriter.h
 *
 *  Created on: 4 juil. 2011 10:47:30
 *      Author: John Stark
 */

#ifndef MEDQTDATAIMAGEWRITER_H_
#define MEDQTDATAIMAGEWRITER_H_

#include "medTestCommonExport.h"

#include <dtkCore/dtkAbstractDataWriter.h>

/** A concrete implementation of a dtkAbstractDataWriter for use in testing. */
class MEDTESTCOMMON_EXPORT medQtDataImageWriter : public dtkAbstractDataWriter
{
    Q_OBJECT;
public:
    medQtDataImageWriter();
    virtual ~medQtDataImageWriter();

    virtual QString description(void) const;
    virtual QStringList handled(void) const;

    static QString s_description();
    static QStringList s_handled();

    virtual QStringList supportedFileExtensions(void) const;

    static bool registered(void);

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

#endif /* MEDQTDATAIMAGEWRITER_H_ */

