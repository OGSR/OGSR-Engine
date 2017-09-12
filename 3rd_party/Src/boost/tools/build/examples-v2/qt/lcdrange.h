/****************************************************************
**
** Definition of LCDRange class, Qt tutorial 7
**
****************************************************************/

#ifndef LCDRANGE_H
#define LCDRANGE_H

#include <qvbox.h>

class QSlider;


class LCDRange : public QVBox
{
    Q_OBJECT
public:
    LCDRange( QWidget *parent=0, const char *name=0 );

    int value() const;

public slots:
    void setValue( int );

signals:
    void valueChanged( int );

private:
    QSlider *slider;
};


#endif // LCDRANGE_H
