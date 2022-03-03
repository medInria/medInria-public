/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

// medInria
#include <medToolBox.h>
#include <viewevent/baseViewEvent.h>


/*! \brief Toolbox to manage list of contours label
 *
 */
class defaultLabelToolBox : public medToolBox
{
    Q_OBJECT

MED_TOOLBOX_INTERFACE("Contours Management",
                      "defaultLabelToolBox",
                      <<"defaultLabelToolBox")

public:
    explicit defaultLabelToolBox(QWidget *parent = nullptr);

    ~defaultLabelToolBox() override;
    static bool registered();
    void clear() override;
    virtual void initialize(QString &seriesName);
    void setName(QString &name);
    virtual void clean();

    virtual void rename(int row, QString &newName);
    virtual void updateItem(medLabelProperty &info);

    void setCurrentViewEventFilter(baseViewEvent *event){ currentViewEvent = event;}

    void selectRow(int position);
    void resetItem(int position, QString &defaultName);

    int getContourPositionAndColor(QString &name, QColor &color);
    void unselectAll();
    void forceItemSelection();

protected slots:

    virtual void onPlusClicked();
    virtual void onMinusClicked();

private:
    void addDefaultItem(QListWidgetItem *item);


protected:

    baseViewEvent *currentViewEvent;

    QColor findAvailableColor(QList<QColor> &colors);
    static QListWidgetItem *createWidgetItem(const QString &name, const QColor &col);
    QStringList getPreLoadNames() const;
    void updatePolygonLabelPosition(int startPosition);

    QLabel *dataName;
    QListWidget* labels;
    QList<QColor> mainColors;
    QPushButton *minusButton;
    QPushButton *plusButton;


};
