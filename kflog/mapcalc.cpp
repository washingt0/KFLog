/***********************************************************************
**
**   mapcalc.cpp
**
**   This file is part of KFLog2.
**
************************************************************************
**
**   Copyright (c):  1999, 2000 by Heiner Lamprecht, Florian Ehinger
**
**   This file is distributed under the terms of the General Public
**   Licence. See the file COPYING for more information.
**
**   $Id$
**
***********************************************************************/

#include <cmath>
#include <mapcalc.h>

#include "mapmatrix.h"

double dist(double lat1, double lon1, double lat2, double lon2)
{
  double pi_180 = PI / 108000000.0;
  double dlat = lat1 - lat2;
  double dlon = lon1 - lon2;

  // lat is used to calculate the earth-radius. We use the average here.
  // Otherwise, the result would depend on the order of the parameters.
  double lat = ( lat1 + lat2 ) / 2.0;

  double dist = RADIUS * sqrt( ( pi_180 * dlat * pi_180 * dlat )
    + ( pi_180 * cos( pi_180 * lat ) * dlon *
        pi_180 * cos( pi_180 * lat ) * dlon ) );

  return dist / 1000.0;
}

double dist(wayPoint* wp1, wayPoint* wp2)
{
  return ( dist( wp1->origP.lat(), wp1->origP.lon(),
                 wp2->origP.lat(), wp2->origP.lon() ) );
}

double dist(wayPoint* wp, flightPoint* fp)
{
  return ( dist( wp->origP.lat(), wp->origP.lon(),
                 fp->origP.lat(), fp->origP.lon() ) );
}

double dist(flightPoint* fp1,  flightPoint* fp2)
{
  return ( dist( fp1->origP.lat(), fp1->origP.lon(),
                 fp2->origP.lat(), fp2->origP.lon() ) );
}

/*
 * Die Funktion scheint noch Probleme zu haben, wenn die Position nahe an
 * 0� W/E liegt.
 */
QString printPos(int coord, bool isLat)
{
  QString pos, posDeg, posMin, posSec;

  int degree = coord / 600000;
  int min = (coord - (degree * 600000)) / 10000;
  int sec = (coord - (degree * 600000) - (min * 10000));
  sec = (sec * 60) / 10000;

  min = (int)sqrt(min * min);
  //if(min < 10)  posMin.sprintf(" 0%d'", min);
  //else
  posMin.sprintf(" %02d'", min);

  sec = (int)sqrt(sec * sec);
  //if(sec < 10)  posSec.sprintf(" 0%d\"", sec);
  //else
  posSec.sprintf(" %02d\"", sec);

  if(isLat)
    {
      if(coord < 0)
        {
          posDeg.sprintf("%02d�", -degree);
          pos = posDeg + posMin + posSec + " S";
        }
      else
        {
          posDeg.sprintf("%02d�", degree);
          pos = posDeg + posMin + posSec + " N";
        }
    }
  else
    {
      if(coord < 0)
        {
          posDeg.sprintf("%03d�", -degree);
          pos = posDeg + posMin + posSec + " W";
        }
      else
        {
          posDeg.sprintf("%03d�", degree);
          pos = posDeg + posMin + posSec + " E";
        }
    }

  return pos;
}

QString printTime(int time, bool isZero, bool isSecond)
{
  QString hour, min, sec;

  int hh = time / 3600;
  int mm = (time - (hh * 3600)) / 60;
  int ss = time - (hh * 3600) - mm * 60;

  if(isZero && hh < 10)  hour.sprintf("0%d", hh);
  else  hour.sprintf("%d", hh);

  if(mm < 10)  min.sprintf("0%d", mm);
  else  min.sprintf("%d", mm);

  if(ss < 10)  sec.sprintf("0%d", ss);
  else  sec.sprintf("%d", ss);

  if(isSecond)  return (hour + ":" + min + ":" + sec);

  return ( hour + ":" + min );
}

float getSpeed(flightPoint p) { return (float)p.dS / (float)p.dT * 3.6; }

float getVario(flightPoint p) { return (float)p.dH / (float)p.dT; }

float getBearing(flightPoint p1, flightPoint p2)
{
  return (float)polar( ( p2.projP.x() - p1.projP.x() ),
                       ( p2.projP.y() - p1.projP.y() ) );
}

double getTrueCourse(WGSPoint p1, WGSPoint p2)
{
  return p1 != p2 ? polar(p1.lat() - p2.lat(), p1.lon() - p2.lon()) * 180.0 / PI : 0.0;
}

double polar(double x, double y)
{
  double angle = 0.0;
  //
  //  dX = 0 ???
  //
  if(x >= -0.001 && x <= 0.001)
    {
      if(y < 0.0) return ( 1.5 * PI );
      else  return ( 0.5 * PI );
    }

  // Punkt liegt auf der neg. X-Achse
  if(x < 0.0)  angle = atan( y / x ) + PI;
  else  angle = atan( y / x );

  // Neg. value not allowed.
  if(angle < 0.0)  angle = 2 * PI + angle;

  if(angle > (2 * PI))  angle = angle - (2 * PI);

  return angle;
}

double int2rad(int deg)
{
  return (double)deg * PI / 108000000.0;
}

double rad2int(double rad)
{
  return (int) (rad * 108000000.0 / PI);
}

double angle(double a, double b, double c)
{
  double a1, b1, c1, tmp;
  a1 = a / RADIUS * 1000.0;
  b1 = b / RADIUS * 1000.0;
  c1 = c / RADIUS * 1000.0;
  
  tmp = (cos(c1) - cos(a1) * cos(b1)) / sin(a1) / sin(b1);
  if (tmp > 1.0) {
    tmp = 1.0;
  }
  else if (tmp < -1.0) {
    tmp = -1.0;
  }

  return acos(tmp);
}

double tc(double lat1, double lon1, double lat2, double lon2)
{
  return fmod(atan2(sin(lon1-lon2)*cos(lat2), 
		    cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lon1-lon2)),
	      2.0 * PI) + PI;
}

WGSPoint posOfDistAndBearing(double lat1, double lon1, double bearing, double dist)
{
  double tmp, lon;
  double tLat, tLon;

  dist = dist / RADIUS * 1000.0;
  tmp = sin(lat1) * cos(dist) + cos(lat1) * sin(dist) * cos(bearing);

  if (tmp > 1.0) {
    tmp = 1.0;
  }
  else if (tmp < -1.0) {
    tmp = -1.0;
  }

  tLat = asin(tmp);
  

  lon = atan2(sin(bearing) * sin(dist) * cos(lat1),
	      cos(dist) - sin(lat1) * (sin(lat1) * cos(dist) + 
				       cos(lat1) * sin(dist) * cos(bearing)));
  tLon = -fmod(lon1 - lon + PI, 2.0 * PI) + PI;

  return WGSPoint(rad2int(tLat), rad2int(tLon));
}

