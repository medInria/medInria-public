#ifndef MEDIMAGESELECTIONWIDGET_H
#define MEDIMAGESELECTIONWIDGET_H

#include <QtGui>

#include "medGuiExport.h"
#include <medDataIndex.h>

class medImageSelectionWidgetPrivate;

class MEDGUI_EXPORT medMultipleImageSelectionWidget : public QDialog
{
    Q_OBJECT

public:
    medMultipleImageSelectionWidget(QList<medDataIndex> previouslySelectedIndexes, QWidget *parent = 0);
    ~medMultipleImageSelectionWidget(void);

    QSize sizeHint() const;
    QList<medDataIndex> getSelectedIndexes();

public slots:
    void clear();

private slots:
    void onPatientSelected(const medDataIndex&);

protected slots:
    void onCancelButtonClicked();
    void onOkButtonClicked();

protected:
    void paintEvent(QPaintEvent* paintEvent);
    void resizeEvent(QResizeEvent* resizeEvent);

private:
    medImageSelectionWidgetPrivate *d;
};

#endif
