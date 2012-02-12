#ifndef VECTORWIDGET_H
#define VECTORWIDGET_H

#include <QHBoxLayout>
#include <QDoubleSpinBox>

class VectorWidget: public QWidget {

    struct SetDouble {

        SetDouble(VectorWidget& w,const unsigned i): ref(w),index(i) { }

        double operator=(const double d) {
            ref.editors[index-1].setValue(d);
            return d;
        }

        VectorWidget& ref;
        const unsigned  index;
    };
    
public:

    VectorWidget(const unsigned n,QWidget* parent=0): QWidget(parent) {
        setContentsMargins(0,0,0,0);
        setAutoFillBackground(true);

        layout  = new QHBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);

        editors = new QDoubleSpinBox[n];
        for (unsigned i=0;i<n;++i)
            editors[i].setParent(this);

        for (unsigned i=0;i<n;++i) {
            setTabOrder(&editors[i],&editors[(i+1)%n]);
            editors[i].setButtonSymbols(QAbstractSpinBox::NoButtons);
            layout->addWidget(&editors[i]);
        }
        editors[0].setFocus(Qt::MouseFocusReason);
    }

    ~VectorWidget() {
        delete[] editors;
        delete   layout;
    }

    SetDouble value(const unsigned i) { return SetDouble(*this,i); }

    double value(const unsigned i) const {
        return editors[i-1].value();
    }

    QHBoxLayout*    layout;
    QDoubleSpinBox* editors;
};

#endif  //  !VECTORWIDGET_H
