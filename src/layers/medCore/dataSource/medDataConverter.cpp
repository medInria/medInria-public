/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include<medDataConverter.h>

medDataConverter *medDataConverter::m_instance = nullptr;

medDataConverter * medDataConverter::instance()
{
    if (m_instance == nullptr)
    {
        m_instance = new medDataConverter(qApp);
    }
    return m_instance;
}

medDataConverter::medDataConverter(QObject * parent) : QObject(parent)
{
}

medDataConverter::~medDataConverter()
{
}

medAbstractData * medDataConverter::import(QString filePath, QString preferedConv)
{
    return nullptr;
}

medAbstractData * medDataConverter::import(QByteArray buffer, QString preferedConv)
{
    return nullptr;
}

QString medDataConverter::exportToFile(medAbstractData * data, QString ext, QString preferedConv)
{
    return QString();
}

QByteArray medDataConverter::exportToStream(medAbstractData * data, QString ext, QString preferedConv)
{
    return QByteArray();
}