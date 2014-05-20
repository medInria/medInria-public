/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <QLabel>

#include <medCoreExport.h>

class medStringListParameter;

class medPoolIndiactorPrivate;
class MEDCORE_EXPORT medPoolIndicator: public QLabel
{
    Q_OBJECT
public:
    medPoolIndicator(QWidget* parent = NULL);
    ~medPoolIndicator();

    void setLinkParameter(medStringListParameter *linkParameter);

public slots:
    void update();

private slots:
    void removeInternLinkParamater();

private:
    medPoolIndiactorPrivate *d;
};
