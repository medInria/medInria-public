/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <medAbstractJob.h>
#include <medPacsExport.h>
#include <medMoveCommandItem.h>

class medPacsMoverPrivate;


class MEDPACS_EXPORT medPacsMover : public medAbstractJob
{
    Q_OBJECT

public:
     medPacsMover(const QVector<medMoveCommandItem>& cmdList);
    ~medPacsMover();


    void doQueuedMove();

signals:
    void import(QString);
   
public slots:
    void onCancel(QObject*);

protected:
    virtual void internalRun();

private slots:
    void progressForward(int);

private:
    medPacsMoverPrivate *d;
};


