#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <medDataIndex.h>
#include <QDialog>

class medExportVideoDialogPrivate;

class medExportVideoDialog: public QDialog
{
    Q_OBJECT

public:
    enum ExportVideoName
    {
        TIME,
        ROTATION,
        SLICE,
    };

    medExportVideoDialog(QWidget *parent, int numberOfFrames, int numberOfSlices);
    virtual ~medExportVideoDialog();

    QVector<int> value();

public slots:      
    void cancel();
    void validate();

protected slots:
    /**
     * @brief adaptWidgetForMethod adapt widgets according to the chosen exporting method
     * @param int: method chosen
     */
    void adaptWidgetForMethod(int method);

private:
    medExportVideoDialogPrivate *d;
};
