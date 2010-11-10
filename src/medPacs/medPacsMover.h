#ifndef MEDPACSMOVER_H
#define MEDPACSMOVER_H

#include "medPacsExport.h"

#include <QtCore>

class medPacsMoverPrivate;
#include "medPacsWidget.h"

class MEDPACS_EXPORT medPacsMover : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medPacsMover(const QVector<medMoveCommandItem>& cmdList);
    ~medPacsMover(void);

    void run(void);

    void doQueuedMove();

signals:
    void import(QString);

    void progressed(int);
    void success (void);
    void failure (void);
    void showError(QObject*,const QString&,unsigned int timeout);

private:
    medPacsMoverPrivate *d;
};

#endif 