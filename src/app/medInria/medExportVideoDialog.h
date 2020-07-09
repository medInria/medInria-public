#pragma once

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

    medExportVideoDialog(QWidget *parent, int numberOfFrames );
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
