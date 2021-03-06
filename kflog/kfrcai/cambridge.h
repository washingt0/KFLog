/***********************************************************************
**
**   cambridge.h
**
**   This file is part of KFLog.
**
************************************************************************
**
**   Copyright (c):  2007 by Hendrik Hoeth
**                   2011-2014 by Axel Pauli
**
**   This file is distributed under the terms of the General Public
**   License. See the file COPYING for more information.
**
**   $Id$
**
***********************************************************************/

#ifndef CAMBRIDGE_H
#define CAMBRIDGE_H

#include <QObject>
#include <QList>
#include <QString>

#include "../frstructs.h"
#include "../flighttask.h"
#include "../flightrecorderpluginbase.h"

class Cambridge : public FlightRecorderPluginBase
{
  Q_OBJECT

 public:

  Cambridge( QObject *parent = 0 );

  virtual ~Cambridge();
  /*
   * Returns the name of the lib.
   */
  virtual QString getLibName() const;
  /*
   * Returns the transfermode this plugin supports.
   */
  virtual TransferMode getTransferMode() const;
  /*
   * get recorder basic data
   */
  virtual int getBasicData(FR_BasicData&);
  /*
   * get recorder config data
   */
  virtual int getConfigData(FR_ConfigData&);
  /**
   * write recorder basic and config data
   */
  virtual int writeConfigData(FR_BasicData&, FR_ConfigData&);
  /*
   * Opens the recorder for serial communication.
   */
  virtual int openRecorder(const QString& portName, int baud);
  /*
   * Closes the connection with the flightrecorder.
   */
  virtual int closeRecorder();
  /*
   * Returns a list of recorded flights in this device.
   */
  virtual int getFlightDir(QList<FRDirEntry*>*);
  /*
   * Download flights from the recorder
   */
  virtual int downloadFlight(int flightID, int secMode, const QString& fileName);
  /*
   * Opens the recorder for other communication.
   */
  virtual int openRecorder(const QString& URL);
  /*
   * Write flight declaration to recorder
   */
  virtual int writeDeclaration(FRTaskDeclaration *taskDecl, QList<Waypoint*> *taskPoints, const QString& name);
  /*
   * Export flight declaration to file
   */
  virtual int exportDeclaration(FRTaskDeclaration *taskDecl, QList<Waypoint*> *taskPoints, const QString& name);
  /*
   * Read waypoint and flight declaration form from recorder into mem
   */
  virtual int readDatabase();
  /*
   * Read tasks from recorder
   */
  virtual int readTasks(QList<FlightTask*> *tasks);
  /*
   * Write tasks to recorder
   */
  virtual int writeTasks(QList<FlightTask*> *tasks);
  /*
   * Read waypoints from recorder
   */
  virtual int readWaypoints(QList<Waypoint*> *waypoints);
  /*
   * Write waypoints to recorder
   */
  virtual int writeWaypoints(QList<Waypoint*> *waypoints);

private:

  /*
   * wait some time in ms
   */
  void wait_ms(const int);

  /*
   * write byte to logger
   */
  int wb(unsigned char c);

  /*
   * send a command to the logger
   */
  int sendCommand(QString cmd);

  /*
   * read data from logger
   */
  unsigned char *readData(unsigned char *buf_p, int count);
  int readReply(QString cmd, int mode, unsigned char *reply);

  /*
   * calculate checksums to check the data received from the logger
   */
  int calcChecksum8(unsigned char *buf, int count);
  int calcChecksum16(unsigned char *buf, int count);

  /*
   * convert latitude and longitude to the format needed for writing waypoints to the logger
   */
  QString lat2cai(int lat);
  QString lon2cai(int lon);
};

#endif

