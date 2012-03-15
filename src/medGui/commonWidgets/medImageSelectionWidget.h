#ifndef MEDIMAGESELECTIONWIDGET_H
#define MEDIMAGESELECTIONWIDGET_H

#include <QtGui>

#include "medGuiExport.h"
#include <medDataIndex.h>

class medImageSelectionWidgetPrivate;

class MEDGUI_EXPORT medImageSelectionWidget : public QDialog
{
    Q_OBJECT

public:
    medImageSelectionWidget(QWidget *parent = 0);
    ~medImageSelectionWidget(void);

    QSize sizeHint(void) const;

public slots:
    void onPatientSelected(const medDataIndex&);

    void clear();

signals:

private:

protected:

private:
    medImageSelectionWidgetPrivate *d;
};

#endif
