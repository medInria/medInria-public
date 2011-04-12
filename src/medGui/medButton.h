#ifndef medButton_h__
#define medButton_h__

#include "medGuiExport.h"

#include <QtGui/QWidget>
#include "medGuiExport.h"

/**
 * General custom button that loads shows an icon
 */
class MEDGUI_EXPORT medButton : public QWidget
{
    Q_OBJECT

public:
    medButton(QWidget *parent, QString resourceLocation, QString toolTip);
    ~medButton(void);

    QSize sizeHint(void) const;

signals:
    void triggered(void);

protected:
    void mousePressEvent(QMouseEvent *event);
};
#endif // medButton_h__
