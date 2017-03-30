#ifndef ADSBSORTER_H
#define ADSBSORTER_H

#include <QWidget>
//#include "trackpoint.h"
#include <QSet>
#include <QMap>



class AdsbTrack
{
public:
    AdsbTrack() {}
    AdsbTrack(qint32 icao) : icaoName(icao) {}
    qint32 icaoName;
    QMap <quint32*, QPair <double, double>* > points; //time, points
};



namespace Ui {
class AdsbSorter;
}

class AdsbSorter : public QWidget
{
    Q_OBJECT

public:
    explicit AdsbSorter(QWidget *parent = 0);
    ~AdsbSorter();

private slots:
    void on_pushButton_released();

    void on_pushButton_2_released();

private:
    Ui::AdsbSorter *ui;


};

#endif // ADSBSORTER_H
