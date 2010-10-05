#ifndef MEDPACSWIDGET_H
#define MEDPACSWIDGET_H

#include <QTreeView>

#include "medPacsExport.h"

class MEDPACS_EXPORT medPacsWidget : public QTreeView
{
    Q_OBJECT

public:
    medPacsWidget(QWidget* parent);
    ~medPacsWidget();

public slots:

    void echoTest();

    void findTest();

};

#endif