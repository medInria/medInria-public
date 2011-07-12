/*
 * msegAlgorithmParametersWidget.h
 *
 *  Created on: 31 mai 2011 13:42:01
 *      Author: jstark
 */

#ifndef MSEGALGORITHMPARAMETERSWIDGET_H_
#define MSEGALGORITHMPARAMETERSWIDGET_H_

#include <QWidget>

namespace mseg {

class Controller;

class AlgorithmParametersWidget : public QWidget {
public:
    AlgorithmParametersWidget( Controller * controller, QWidget * parent ) : 
        QWidget(parent), 
            m_controller(controller) {}
        virtual ~AlgorithmParametersWidget() {}

protected:
    Controller * controller() { return m_controller; }

private:
    Controller * m_controller;
};

}

#endif /* MSEGALGORITHMPARAMETERSWIDGET_H_ */

