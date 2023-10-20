#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QString>
#include <medAbstractData.h>

class medAbstractDataReader;
class medAbstractDataWrite;

class medDataConverter : public QObject
{
    Q_OBJECT

public:
    static medDataConverter * instance();
    ~medDataConverter();

    medAbstractData* import(QString filePath,  QString preferedConv = "");
    medAbstractData* import(QByteArray buffer, QString preferedConv = "");


    QString    exportToFile  (medAbstractData* data, QString ext, QString preferedConv = "");
    QByteArray exportToStream(medAbstractData* data, QString ext, QString preferedConv = "");

private:
    medDataConverter(QObject *parent);

    QMap<QString,    QList<medAbstractDataReader*> > m_fileExt2Reader;
    QMap<QByteArray, QList<medAbstractDataReader*> > m_fileHeader2Reader;

    QMap<QString,    QList<medAbstractDataWrite*> > m_fileExt2Writer;
    QMap<QByteArray, QList<medAbstractDataWrite*> > m_fileHeader2Writer;

    QMap<QString, medAbstractDataReader*> m_Converter2Reader;
    QMap<QString, medAbstractDataWrite*>  m_Converter2Writer;

    static medDataConverter *m_instance;

};