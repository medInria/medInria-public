#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QString>

class medAbstractAnnotation
{
public:
    virtual void setUrl(const QString &url) { m_url = url;};
    virtual int isServerAvailable() = 0;
    virtual QList<QMap<QString, QString>> findAnnotationMinimalEntries(const QString &studyInstanceUID) = 0;

protected:
    QString m_url;
};