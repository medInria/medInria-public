#ifndef MEDPACSMOVER_H
#define MEDPACSMOVER_H

#include "medPacsExport.h"

#include <QtCore>

class medPacsMoverPrivate;

class MEDPACS_EXPORT medPacsMover : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medPacsMover(int group, int elem, QString query, QString storageFolder, int nodeIndex);
    ~medPacsMover(void);

    void run(void);

    void doMove();

protected:
    void readSettings(void);

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