#ifndef MEDIMAGESELECTIONWIDGET_H
#define MEDIMAGESELECTIONWIDGET_H

#include <QtGui>

#include "medGuiExport.h"

class medImageSelectionWidgetPrivate;

class MEDGUI_EXPORT medImageSelectionWidget : public QDialog
{
    Q_OBJECT

public:
    medImageSelectionWidget(QWidget *parent = 0);
    ~medImageSelectionWidget(void);

    QSize sizeHint(void) const;

public:

public slots:
    void clear();

signals:

private:

protected:

private:
    medImageSelectionWidgetPrivate *d;
};

#endif
