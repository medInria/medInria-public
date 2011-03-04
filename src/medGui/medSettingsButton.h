#ifndef MEDSETTINGSBUTTON_H
#define MEDSETTINGSBUTTON_H


#include "medGuiExport.h"

#include <QtGui/QWidget>
#include "medGuiExport.h"

class MEDGUI_EXPORT medSettingsButton : public QWidget
{
    Q_OBJECT

public:
     medSettingsButton(QWidget *parent = NULL);
    ~medSettingsButton(void);

    QSize sizeHint(void) const;

signals:
    void editSettings(void);

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // MEDSETTINGSBUTTON_H
