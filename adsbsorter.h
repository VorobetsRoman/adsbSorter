#ifndef ADSBSORTER_H
#define ADSBSORTER_H

#include <QWidget>
//#include "trackpoint.h"
#include <QSet>
#include <QMap>
#include <QFile>



class AdsbTrack
{
public:
    AdsbTrack() {}
    AdsbTrack(qint32 icao) : icaoName(icao) {}
    qint32 icaoName;
    QMap <quint32, QPair <double, double>* > points; //time, points
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
    void on_pbOpen_released();
    void on_pbStart_released();

private:
    Ui::AdsbSorter  *ui {NULL};
    QString         inFileName {""};

    void sortData       (QMap <qint32, AdsbTrack*> *trackList);
    void writeOutFile   (QFile *outFile, QMap <qint32, AdsbTrack*> *trackList);
};

#endif // ADSBSORTER_H
