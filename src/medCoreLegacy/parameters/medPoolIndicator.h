/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QLabel>

#include <medCoreExport.h>

class medStringListParameter;

class medPoolIndiactorPrivate;
class MEDCORE_EXPORT medPoolIndicator: public QWidget
{
    Q_OBJECT

public:
    medPoolIndicator(QWidget* parent = NULL);
    ~medPoolIndicator();

    void addColorIndicator(QColor, QString);
    void removeColorIndicator(QColor);

public slots:
    void replaceColorIndicator(QColor, QColor);

private:
    medPoolIndiactorPrivate *d;
};
