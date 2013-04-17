/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkDataDiffusionGradientList.h"

#include <dtkCore/dtkAbstractDataFactory.h>

#include <QFile>
#include <QVector>
#include <QString>

// /////////////////////////////////////////////////////////////////
// itkDataDiffusionGradientListPrivate
// /////////////////////////////////////////////////////////////////

class itkDataDiffusionGradientListPrivate
{
public:

    typedef QVector<double>       GradientType;
    typedef QVector<GradientType> GradientListType;

    GradientListType gradients;
};

// /////////////////////////////////////////////////////////////////
// itkDataDiffusionGradientList
// /////////////////////////////////////////////////////////////////

itkDataDiffusionGradientList::itkDataDiffusionGradientList() : dtkAbstractData(), d(new itkDataDiffusionGradientListPrivate)
{

}

itkDataDiffusionGradientList::~itkDataDiffusionGradientList()
{

}

bool itkDataDiffusionGradientList::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataDiffusionGradientList", createItkDataDiffusionGradientList);
}

QString itkDataDiffusionGradientList::description() const
{
    return "itkDataDiffusionGradientList";
}

void *itkDataDiffusionGradientList::data()
{
    return d->gradients.data();
}

void *itkDataDiffusionGradientList::data (int channel)
{
    if (channel>=d->gradients.count())
        return 0;
    return d->gradients[channel].data();
}

void itkDataDiffusionGradientList::setData (void* data)
{
    d->gradients = *(dynamic_cast<itkDataDiffusionGradientListPrivate::GradientListType*>( static_cast<QObject*>(data) ));
}

bool itkDataDiffusionGradientList::read (const QString& filename)
{
    typedef itkDataDiffusionGradientListPrivate::GradientType     GradientType;
    typedef itkDataDiffusionGradientListPrivate::GradientListType GradientListType;
    

    QFile textfile( filename );
    if (!textfile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    
    QVector<QString> file;
    QTextStream in( &textfile );
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        file.push_back ( line );
    }


    QVector<QString> aux;
    for( int i=0; i<file.size(); i++) {
        QString line = file[i].trimmed();
        if( line.length()>0 && line[0]!='#' ) {
            aux.push_back (line);
        }
    }
    file = aux;


    QVector< QVector<double> > numbers;
    int total = 0;
    for( int i=0; i<file.size(); i++) {

        QStringList num_list = file[i].split (" ", QString::SkipEmptyParts);

        QVector< double > num;
        for( int j=0; j<num_list.size(); j++) {
            bool ok = false;
            double val = num_list[j].toDouble ( &ok );
            if( !ok ) {
                qDebug() << "Unexpected character when parsing line: " << file[i].toAscii().constData();
                return false;
            }
            num.push_back ( val );
            total++;
        }
        numbers.push_back ( num );
    }


    GradientListType output;
    
    if( total%3==0 ) { // there are 3xsomething numbers
        // 2 cases: either Nx3 or 3xN:
        if ( numbers.size()==3 ) {
            if( numbers[0].size()!=numbers[1].size() && numbers[0].size()!=numbers[2].size() ){
                qDebug() << "Error: Lines have not the same length.";
                return false;
            }

            for( int i=0; i<numbers[0].size(); i++) {
                GradientType grad (3);
                grad[0] = numbers[0][i];
                grad[1] = numbers[1][i];
                grad[2] = numbers[2][i];

                output.push_back (grad);
            }
        }
        else {
            for( int i=0; i<numbers.size(); i++) {
                if (numbers[i].size() != 3 ){
                    qDebug() << "Error: There should be 3 gradients per line, check the file.";
                    return false;
                }

                GradientType grad (3);
                grad[0] = numbers[i][0];
                grad[1] = numbers[i][1];
                grad[2] = numbers[i][2];

                output.push_back (grad);
            }
        }
    }
    else if ( (total-1)%3==0 ) {// we assume here that there is a number giving the total number of gradients to read
        double numOfGrad = numbers[0][0];
        if( numOfGrad != (double)(numbers.size())-1 ) {
            char Message[512];
            sprintf( Message, "Error: The number of gradients (%d) does not match the actual number of lines (%d).", (int)numOfGrad, (int)numbers.size()-1);
            qDebug() << Message;
            return false;
        }
        for( int i=1; i<numbers.size(); i++) {
            if (numbers[i].size() != 3 ){
                qDebug() << "Error: There should be 3 gradients per line, check the file.";
                return false;
            }

            GradientType grad (3);
            grad[0] = numbers[i][0];
            grad[1] = numbers[i][1];
            grad[2] = numbers[i][2];

            output.push_back (grad);
        }
    }
    else {
        qDebug() << "Error: Unrecognized file format.";
        return false;
    }
    

    /*
    // now check if the first gradient is [0,0,0]
    GradientType grad = output[0];
    if( grad[0]==0.0 && grad[1]==0.0 && grad[2]==0.0 ){

      // we remove the null gradient
      GradientListType listG ( output.size() - 1 );
      for( int i=1; i<output.size(); i++) {
        listG[i-1] = output[i];
      }
      output = listG;
    }
   */

    this->setNumberOfChannels (output.size());

    d->gradients = output;

    return true;    
}

bool itkDataDiffusionGradientList::write (const QString& file)
{
    qDebug() << "itkDataDiffusionGradientList::write() not implemented yet.";
    return false;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractData *createItkDataDiffusionGradientList()
{
    return new itkDataDiffusionGradientList;
}
