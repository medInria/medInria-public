#pragma once

#include <medCoreExport.h>

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

class MEDCORE_EXPORT medAttachedPopupWidget : public QWidget
{
    Q_OBJECT
public:
    enum Orientation {TOP, BOTTOM, LEFT, RIGHT};
    Q_PROPERTY(medAttachedPopupWidget::Orientation m_orientation READ orientation WRITE setOrientation)

    medAttachedPopupWidget(QWidget *parent = 0);
    void attachTo(QWidget * w, medAttachedPopupWidget::Orientation orientation = TOP);
    QWidget * centerWidget();

    medAttachedPopupWidget::Orientation orientation() const;

public slots:
    void display();

    void setOrientation(medAttachedPopupWidget::Orientation arg);

protected:
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    void placeArrow();

    QWidget * m_hostWidget;
    QScopedPointer<QGridLayout> m_gridLayout;
    QScopedPointer<QWidget> m_center;
    QScopedPointer<QPixmap> m_arrow;
    QScopedPointer<QLabel> m_arrowLabel;
    Orientation m_orientation;
};
