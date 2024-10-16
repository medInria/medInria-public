#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QString>
#include <QUrl>
#include <QMap>

class medFirstStartJSonIO : public QObject
{
    Q_OBJECT
public:
    medFirstStartJSonIO(QString path);
    ~medFirstStartJSonIO();

    bool readJsonUpdatedFile();
    bool writeJsonUpdatedFile();

    QMap <QString, QPair <QString, QUrl> > getUpdatedMap() { return m_updatedMap; }

private:
    QString m_confPath;
    QString m_baseUpdatedPath;
    QMap <QString, QPair <QString, QUrl> > m_registerdMap;
    QMap <QString, QPair <QString, QUrl> > m_updateMap;
    QMap <QString, QPair <QString, QUrl> > m_updatedMap;
};

