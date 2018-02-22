/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QLabel>

#include <medCoreLegacyExport.h>

class medStringListParameterL;

class medPoolIndiactorPrivate;
class MEDCORELEGACY_EXPORT medPoolIndicatorL: public QWidget
{
    Q_OBJECT

public:
    medPoolIndicatorL(QWidget* parent = NULL);
    ~medPoolIndicatorL();

    void addColorIndicator(QColor, QString);
    void removeColorIndicator(QColor);

public slots:
    void replaceColorIndicator(QColor, QColor);

private:
    medPoolIndiactorPrivate *d;
};
