#pragma once

#include <QWidget>

class medDatabaseCompactWidgetPrivate;
class medDatabaseView;
class medDatabasePreview;


/**
 * @brief
 */
class medDatabaseCompactWidget: public QWidget
{
    Q_OBJECT
public :
    medDatabaseCompactWidget(QWidget *parent = NULL);
    virtual ~medDatabaseCompactWidget();

    void setView(medDatabaseView *view);
    void setPreview(medDatabasePreview *preview);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    medDatabaseCompactWidgetPrivate *d;
};


