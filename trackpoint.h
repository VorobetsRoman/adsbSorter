#ifndef TRACKPOINT_H
#define TRACKPOINT_H

#include <QtGlobal>
#include <QPair>
#include <QMap>


/*class TrackPoint
{
public:
    TrackPoint() {}
    TrackPoint(quint32 *u_time, qint32  *u_icao, double u_lat, double u_lon) :
            time (u_time), icao (u_icao), lat (u_lat), lon (u_lon) {}
    quint32 *time;
    qint32  *icao;
    double lat;
    double lon;
};*/


class AdsbTrack
{
public:
    AdsbTrack() {}
    qint32 icaoName;
    qint64 personalName;
    QMap <quint32*, QPair <double, double>* > points;
};


#endif // TRACKPOINT_H
