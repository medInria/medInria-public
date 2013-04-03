#ifndef MEDPACSMOVER_H
#define MEDPACSMOVER_H


#include <medJobItem.h>
#include "medPacsExport.h"
#include "medMoveCommandItem.h"

class medPacsMoverPrivate;


class MEDPACS_EXPORT medPacsMover : public medJobItem
{
    Q_OBJECT

public:
     medPacsMover(const QVector<medMoveCommandItem>& cmdList);
    ~medPacsMover();

    void run();

    void doQueuedMove();

signals:
    void import(QString);
   
public slots:
    void onCancel(QObject*);

private slots:
    void progressForward(int);

private:
    medPacsMoverPrivate *d;
};

#endif 
