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

#include <functional>

class medFirstStartUpdateFilePrivate;
class medFirstStartUpdateFile : public QObject
{
public:
    medFirstStartUpdateFile(QString filePath, QString featureName, QString originalFilePath,
                            std::function<bool(QString, QString)> update = nullptr, std::function<bool(QString, QString)> init = nullptr);
    ~medFirstStartUpdateFile();

    bool installFile();

private:
    bool prepareFile();
    bool curentFileOutdated();

private:
    medFirstStartUpdateFilePrivate *d;
};

