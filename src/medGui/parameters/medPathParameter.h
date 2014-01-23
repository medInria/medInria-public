/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>

#include "medGuiExport.h"

class QSize;
class QWidget;
class QIcon;

class medPathParameterPrivate;
class MEDGUI_EXPORT medPathParameter: public medAbstractStringParameter
{
    Q_OBJECT

public:
    medPathParameter(QString name = "Unknow path parameter");
    virtual ~medPathParameter();

    void setButtonText(QString& item);
    void setButtonIcon(QIcon& icon);
    void setButtonIconSize(QSize& size);
    void clear();
    void setExploratorCaption(QString& caption);
    void setExploratorFilter(QString& filter);
    void setExplorFile(bool explorFile);
    void setExplorDirectory(bool explorDirectory);

    QStringList& items() const;

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private slots:
    void openExplorator();
    void removeInternWidget();

private:
    medPathParameterPrivate* d;
};
