#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QLabel>

#include <medCoreExport.h>

class medStringListParameterL;

class medPoolIndiactorPrivate;
class MEDCORE_EXPORT medPoolIndicatorL: public QWidget
{
    Q_OBJECT

public:
    medPoolIndicatorL(QWidget* parent = nullptr);
    ~medPoolIndicatorL();

    void addColorIndicator(QColor, QString);
    void removeColorIndicator(QColor);

public slots:
    void replaceColorIndicator(QColor, QColor);

private:
    medPoolIndiactorPrivate *d;
};
