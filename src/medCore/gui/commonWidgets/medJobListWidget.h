/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtGui/QWidget>

class medAbstractJob;
class medJobListWidgetPrivate;

/**
 * @class medJobListWidget
 * @brief Creates JobWidgets that represent the progression of a jobItem.
 * All visible jobs will be managed here. The class stores pointers in hashTables to identify the sender of status messages
 * It provides methods to communicate with the jobItems (e.g. to cancel the job)
 */
class MEDCORE_EXPORT medJobListWidget : public QWidget
{
    Q_OBJECT

public:
     medJobListWidget(QWidget *parent = 0);
    ~medJobListWidget();

public slots:
    void addJob(medAbstractJob* job, QString label);
    void removeJob(medAbstractJob *job);
    void setJobProgress(int progress);
    void setJobSuccess();
    void setJobFailure();
    void displayJobError(QString errorMessage);

    void disableJobCancel();
    void cancelJob();

private slots:
    void removeJobWidgets();

private:
    medJobListWidgetPrivate *d;
};


