/***********************************************************************
**
**   flarm.cpp
**
**   This file is part of KFLog4.
**
************************************************************************
**
**   Copyright (c):  2011 by Eggert Ehmke
**
**   This file is distributed under the terms of the General Public
**   License. See the file COPYING for more information.
**
**   $Id: flarm.cpp 1236 2011-06-27 20:05:37Z axel $
**
***********************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <ctype.h>
#include <cmath>
#include <string.h>

#include <QtCore>
#include <QFileDialog>

#include "flarm.h"

#define MAX_LSTRING    63

/*logger defines*/

// for the flarmcfg file we need DOS line feeds
#define ENDL "\r\n"

/**
 * The device-name of the port.
 */
const char* portName = '\0';
int portID = -1;
const char* c36 = "0123456789abcdefghijklmnopqrstuvwxyz";

/**
 * holds the port-settings at start of the application
 */
struct termios oldTermEnv;

/**
 * is used to change the port-settings
 */
struct termios newTermEnv;

/*
 * Command bytes for communication with the lx device
 */
unsigned char STX = 0x02, /* Command prefix like AT for modems        */
  ACK = 0x06,      /* Response OK, if the crc check is ok             */
  NAK = 0x15,      /* Response not OK, if the crc check is not ok     */
  SYN = 0x16,      /* Request for CONNECT                             */
  K = 'K' | 0x80,  /* get_extra_data()   - trailing fix sized block   */
  L = 'L' | 0x80,  /* get_mem_sections() - the flight data is         */
                   /*                      retrieved in blocks        */
  M = 'M' | 0x80,  /* getFlightDir()-      table of flights           */
  N = 'N' | 0x80,  /* def_mem()          - memory range of one flight */
  Q = 'Q' | 0x80,  /* read_mem_setting()                              */
  R = 'R',         /* readWaypoints()                                 */
  W = 'W',         /* writeWaypoints()                                */
  f = 'f' | 0x80;  /* get_logger_data()  - first block                */
                   /* f++ get_logger_data()  - next block             */

  char manufactureShortKey = 'X';
  char manufactureKey[] = "xxx";  // Let's start with an empty key. If 'xxx'
                                  // appears, then reading the 'A'-record
                                  // failed.

  unsigned char *memContents; /* buffer to hold igc contents */
  int contentSize;            /* length of igc file buffer   */


/**
 * Needed to reset the serial port in any case of unexpected exiting
 * of the program. Called via signal-handler of the runtime-environment.
 */
void releaseTTY(int /* signal*/)
{
  tcsetattr(portID, TCSANOW, &oldTermEnv);
}

Flarm::Flarm( QObject *parent ) : FlightRecorderPluginBase( parent )
{
  //Set flight recorders capabilities. Defaults are 0 and false.
  //_capabilities.maxNrTasks = TASK_MAX;             //maximum number of tasks
  _capabilities.maxNrTasks = 1;                      //maximum number of tasks
  _capabilities.maxNrWaypoints = WAYPOINT_MAX;       //maximum number of waypoints
  _capabilities.maxNrWaypointsPerTask = MAXTSKPNT;   //maximum number of waypoints per task
  _capabilities.maxNrPilots = 1;                     //maximum number of pilots
  _capabilities.transferSpeeds = bps04800 |          //supported transfer speeds
                                 bps09600 |
                                 bps19200 |
                                 bps38400 |
                                 bps57600;

  _capabilities.supDlWaypoint = false;       //supports downloading of waypoints?
  _capabilities.supUlWaypoint = false;       //supports uploading of waypoints?
  _capabilities.supDlFlight = false;         //supports downloading of flights?
  _capabilities.supUlFlight = false;         //supports uploading of flights?
  _capabilities.supSignedFlight = false;     //supports downloading in of signed flights?
  _capabilities.supDlTask = false;           //supports downloading of tasks? no
  _capabilities.supUlTask = true;            //supports uploading of tasks?
  _capabilities.supExportDeclaration = true; //supports export of declaration?
  _capabilities.supUlDeclaration = false;    //supports uploading of declarations?
  _capabilities.supDspSerialNumber = true;
  _capabilities.supDspRecorderType = false;
  _capabilities.supDspPilotName = true;
  _capabilities.supDspCoPilotName = true;
  _capabilities.supDspGliderType = true;
  _capabilities.supEditPilotName = true;
  _capabilities.supEditCoPilotName = true;
  _capabilities.supDspGliderType = true;
  _capabilities.supDspGliderID = true;
  _capabilities.supDspCompetitionID = true;
  _capabilities.supAutoSpeed = true;       //supports automatic transfer speed detection
  //End set capabilities.

  portID = -1;

}

Flarm::~Flarm()
{
  closeRecorder();
  qDeleteAll( flightIndex );
}

/**
 * Returns the transfer mode this plugin supports.
 */
FlightRecorderPluginBase::TransferMode Flarm::getTransferMode() const
{
  return FlightRecorderPluginBase::serial;
}

int Flarm::getFlightDir( QList<FRDirEntry *>* dirList )
{
  Q_UNUSED(dirList)

  qDebug ("Flarm::getFlightDir");

  return FR_NOTSUPPORTED;
}

QString Flarm::getFlarmData (QFile& file, const QString& cmd, const QString& key) {
  QString str = cmd + ",R," + key + "*";
  ushort cs = calcCheckSum (str.length(), str);
  QString ccs = QString ("%1").arg (cs, 2, 16, QChar('0'));
  QString sentence = str + ccs + ENDL;
  qDebug () << sentence << endl;
  file.write (sentence.toLatin1().constData(), sentence.length());
  file.flush();
    
  QString bytes = file.readLine();
  //sometimes some other sentences come inbetween
  time_t t1 = time(NULL);
  while (!bytes.startsWith (cmd + ",A,")) {
    if (time(NULL) - t1 > 10) {
      qDebug () << "No response from recorder within 10 seconds!" << endl;
      return "";
    }
    qDebug () << "ignored bytes: " << bytes << endl;
    bytes = file.readLine();
  }
  // qDebug () << "bytes: " << bytes;

  QStringList list = bytes.split("*");
  QString answer = list[0];
  QString checksum = list[1];
  bool ok;
  cs = checksum.toInt (&ok, 16);
  if (!ok) {
    qDebug () << "checksum not readable: " << checksum << endl;
    return "";
  }
  // qDebug () << "checksum valid" << endl;
  if (cs == calcCheckSum (answer.length(), answer)) {
    // qDebug () << "checksum ok" << endl;
    list = answer.split(",");
    return list[3];
  }
  else {
    qDebug () << "bad Checksum: " << bytes << "; " << checksum << endl;
    return "";
  }
}

bool Flarm::putFlarmData (QFile& file, const QString& cmd, const QString& key, const QString& data) {
  QString str = cmd + ",S," + key + "," + data + "*";
  ushort cs = calcCheckSum (str.length(), str);
  QString ccs = QString ("%1").arg (cs, 2, 16, QChar('0'));
  QString sentence = str + ccs + ENDL;
  qDebug () << sentence << endl;
  file.write (sentence.toLatin1().constData(), sentence.length());
  file.flush();

  QString bytes = file.readLine();
  //sometimes some other sentences come inbetween
  time_t t1 = time(NULL);
  while (!bytes.startsWith (cmd + ",A,")) {
    if (time(NULL) - t1 > 10) {
      qDebug () << "No response from recorder within 10 seconds!" << endl;
      return false;
    }
    qDebug () << "ignored bytes: " << bytes << endl;
    bytes = file.readLine();
  }
  qDebug () << "putFlarmData: " << bytes << endl;

  QStringList list = bytes.split("*");
  QString answer = list[0];
  QString checksum = list[1];
  bool ok;
  cs = checksum.toInt (&ok, 16);
  if (!ok) {
    qDebug () << "checksum not readable: " << checksum << endl;
    return "";
  }
  // qDebug () << "checksum valid" << endl;
  if (cs == calcCheckSum (answer.length(), answer)) {
    return true;
  }
  else {
    qDebug () << "bad Checksum: " << bytes << "; " << checksum << endl;
    return false;
  }
}

/**
  * This function retrieves the basic recorder data from the flarm device
  * currently supported are: serial number, devive type, pilot name, glider type, glider id, competition id.
  * Written by Eggert Ehmke <eggert.ehmke@berlin.de>, <eggert@kflog.org>
  */
int Flarm::getBasicData(FR_BasicData& data)
{
  // TODO: adapt to FLARM
  qDebug ("Flarm::getBasicData");
  
  if (!check4Device()) {
    return FR_ERROR;
  }

  QFile file;
  file.open (portID, QIODevice::ReadWrite);
  
  data.pilotName     = getFlarmData (file, "$PFLAC","PILOT");
  data.copilotName   = getFlarmData (file, "$PFLAC","COPIL");
  data.gliderType    = getFlarmData (file, "$PFLAC","GLIDERTYPE");
  data.gliderID      = getFlarmData (file, "$PFLAC","GLIDERID");
  data.competitionID = getFlarmData (file, "$PFLAC","COMPID");
  data.serialNumber  = getFlarmData (file, "$PFLAC","ID");

  return FR_OK;
}

int Flarm::getConfigData(FR_ConfigData& /*data*/)
{
  return FR_NOTSUPPORTED;
}

int Flarm::writeConfigData(FR_BasicData& data, FR_ConfigData& /*configdata*/)
{
  qDebug ("Flarm::writeConfigData");
  
  if (!check4Device()) {
    return FR_ERROR;
  }
  
  QFile file;
  file.open (portID, QIODevice::ReadWrite);

  if (!putFlarmData (file, "$PFLAC", "PILOT", data.pilotName))
    return FR_ERROR;
  if (!putFlarmData (file, "$PFLAC", "COPIL", data.copilotName))
    return FR_ERROR;
  if (!putFlarmData (file, "$PFLAC", "GLIDERTYPE", data.gliderType))
    return FR_ERROR;
  if (!putFlarmData (file, "$PFLAC", "GLIDERID", data.gliderID))
    return FR_ERROR;
  if (!putFlarmData (file, "$PFLAC", "COMPID", data.competitionID))
    return FR_ERROR;

  return FR_OK;
}

int Flarm::downloadFlight(int /*flightID*/, int /*secMode*/, const QString& /*fileName*/)
{
  qDebug ("Flarm::downloadFlight");

  return FR_NOTSUPPORTED;
}

int Flarm::openRecorder(const QString& pName, int baud)
{
  //TODO: adapt to FLARM
  portName = pName.toLatin1().data();

  portID = open(portName, O_RDWR | O_NOCTTY);

  if(portID != -1) {
    //
    // Before we change any port-settings, we must establish a
    // signal-handler, which is used to restore the port-settings
    // after terminating the program.
    // Because a SIGKILL-signal removes the program immediately,
    // the status of the port will be undefined.
    //
    struct sigaction sact;

    sact.sa_handler = releaseTTY;
    sigaction(SIGHUP, &sact, NULL);
    sigaction(SIGINT, &sact, NULL);
    sigaction(SIGPIPE, &sact, NULL);
    sigaction(SIGTERM, &sact, NULL);

    /*
     * Set the terminal mode of the serial line
     */

    // reading the current port-settings
    tcgetattr(portID, &newTermEnv);

    // storing the port-settings to restore them ...
    oldTermEnv = newTermEnv;

    /*
     * Do some common settup
     */
    newTermEnv.c_iflag = IGNPAR;
    newTermEnv.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    newTermEnv.c_oflag &= ~OPOST;
    newTermEnv.c_oflag |= ONLCR;
    newTermEnv.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    /*
     * No flow control at all :-(
     */
    newTermEnv.c_cflag &= ~(CSIZE | PARENB | CSTOPB | CRTSCTS | IXON | IXOFF);
    newTermEnv.c_cflag |= (CS8 | CLOCAL | CREAD);

    // control characters
    newTermEnv.c_cc[VMIN] = 0; // don't wait for a character
    newTermEnv.c_cc[VTIME] = 1; // wait at least 1 msec.

//  4800 - 57600 bps
//  These are the only speeds known by Flarm devices
//  Taken from Data port Specification
    if (baud >= 57600)     _speed = B57600;
    else if(baud >= 38400) _speed = B38400;
    else if(baud >= 19200) _speed = B19200;
    else if(baud >=  9600) _speed = B9600;
    else                   _speed = B4800;

    cfsetospeed(&newTermEnv, _speed);
    cfsetispeed(&newTermEnv, _speed);

    // flush the device
    tcflush (portID, TCIOFLUSH);
    // Activating the port-settings
    tcsetattr(portID, TCSANOW, &newTermEnv);

    _isConnected = true;
    //_da4BufferValid = false;

    if(!AutoBaud()){
      qWarning() << QObject::tr("No baudrate found!");
      _isConnected = false;
      return FR_ERROR;
    };

    //_keepalive->start (1000); // one second timer
    return FR_OK;
    }
  else {
    qWarning() << QObject::tr("No logger found!");
    _isConnected = false;
    return FR_ERROR;
  }
}


/**
  * this method copied from Cumulus
  * NMEA-0183 Standard
  * The optional checksum field consists of a "*" and two hex digits
  * representing the exclusive OR of all characters between, but not
  * including, the "$" and "*".  A checksum is required on some sentences.
  */
ushort Flarm::calcCheckSum (int pos, const QString& sentence)
{
  ushort sum = 0;

  for( int i=1; i < pos; i++ ) {
    ushort c = (sentence[i]).toAscii();

    if( c == '$' ) // Start sign will not be considered
      continue;

    if( c == '*' ) // End of sentence reached
      break;

    sum ^= c;
  }

  return sum;
}

/**
  * This method copied from Cumulus
  * This method checks if the checksum in the sentence matches the sentence. It retuns true if it matches, and false otherwise.
  */
bool Flarm::checkCheckSum(int pos, const QString& sentence)
{
  ushort check = (ushort) sentence.right(2).toUShort(0, 16);
  return (check == calcCheckSum (pos, sentence));
}

/**
 * Check presence of FLARM-device and make CONNECT
 *
 */
bool Flarm::AutoBaud()
{
  //TODO: adapt to FLARM
  speed_t autospeed;
  int     autobaud = 57600;
  bool rc = false;
  time_t t1;
  _errorinfo = "";

  t1 = time(NULL);
  while (true) {
    tcflush(portID, TCIOFLUSH); // Make sure the next ACK comes from the
                                // following wb(SYN). And remove the
                                // position data, that might have been
                                // arrived.
    QFile file;
    file.open (portID, QIODevice::ReadOnly);
    
    QString bytes = file.readLine();
    qDebug () << "bytes: " << bytes;

    if (bytes.contains (QRegExp("^\\$PFLAU|^\\$GPGGA|^\\$PGRMZ|^\\$GPRMC"))) {
      rc = true;
      break;
    }
    else {
      // waiting 10 secs. for response
      // qDebug ("ret = %x", ret);
      if (time(NULL) - t1 > 10) {
        _errorinfo = tr("No response from recorder within 10 seconds!\n");
        rc = false;
        break;
      }
    }

    //
    // ( - Christian - )
    //
    // Autobauding :-)
    //
    // this way we do autobauding each time this function is called.
    // Shouldn't we do it in OpenRecorder?
    if     (autobaud >= 57600) { autobaud = 38400; autospeed = B57600; }
    else if(autobaud >= 38400) { autobaud = 19200; autospeed = B38400; }
    else if(autobaud >= 19200) { autobaud =  9600; autospeed = B19200; }
    else if(autobaud >=  9600) { autobaud =  4800; autospeed = B9600; }
    else                       { autobaud = 57600; autospeed = B4800; }

    cfsetospeed(&newTermEnv, autospeed);
    cfsetispeed(&newTermEnv, autospeed);
    if (_speed != autospeed)
    {
      _speed = autospeed;
      switch (_speed)
      {
        case B4800:
          emit newSpeed (4800);
          qDebug ("autospeed: %d", 4800);
          break;
        case B9600:
          emit newSpeed (9600);
          qDebug ("autospeed: %d", 9600);
          break;
        case B19200:
          emit newSpeed (19200);
          qDebug ("autospeed: %d", 19200);
          break;
        case B38400:
          emit newSpeed (38400);
          qDebug ("autospeed: %d", 38400);
          break;
        case B57600:
          emit newSpeed (57600);
          qDebug ("autospeed: %d", 57600);
          break;
        default:
          qDebug ("autospeed: illegal value");
      }
    }

    tcsetattr(portID, TCSANOW, &newTermEnv);

  }
  return rc;
}

/**
 * Check presence of Flarm-device and make CONNECT
 *
 */
bool Flarm::check4Device()
{
  _errorinfo = "";
  
  QFile file;
  file.open (portID, QIODevice::ReadWrite);

  time_t t1 = time(NULL);
  while (true) {
    QString result = getFlarmData (file, "$PFLAC","ID");
    if (result.isEmpty()) {
      _errorinfo = tr("No response from flarm device!\n");
      return false;
    }
    else
      break;
    // waiting 10 secs. for response
    if (time(NULL) - t1 > 10) {
      _errorinfo = tr("No response from flarm device within 10 seconds!\n");
      return false;
    }
  }
  return true;
}

int Flarm::closeRecorder()
{
  if( portID != -1 )
    {
      tcsetattr( portID, TCSANOW, &oldTermEnv );
      close( portID );
      portID = -1;
      _isConnected = false;
      return FR_OK;
    }
  else
    {
      return FR_ERROR;
    }
}

/**
  * create lat string as defined by flarm docu
  * @Author: eggert.ehmke@berlin.de
  */
QString Flarm::lat2flarm(int lat)
{
  QString hemisphere = (lat>=0) ? "N" : "S";
  lat = abs(lat);

  int deg, min, sec;
  WGSPoint::calcPos (lat, deg, min, sec);
  // in Flarm spec this is defined as 1/1000 minutes.
  int dec = (sec / 60.0) * 1000;

  QString result = QString().sprintf("%02d%02d%03d", deg, min, dec);
  result += hemisphere;
  return result;
}

/**
  * create lon string as defined by flarm docu
  * @Author: eggert.ehmke@berlin.de
  */
QString Flarm::lon2flarm(int lon)
{
  QString hemisphere = (lon>=0) ? "E" : "W";
  lon = abs(lon);
  int deg, min, sec;
  WGSPoint::calcPos (lon, deg, min, sec);
  // in Flarm spec this is defined as 1/1000 minutes.
  int dec = (sec / 60.0) * 1000;
  QString result = QString().sprintf("%03d%02d%03d", deg, min, dec);
  result += hemisphere;
  return result;
}

int Flarm::writeDeclaration(FRTaskDeclaration* , QList<Waypoint*>* )
{
  return FR_NOTSUPPORTED;
}

/**
  * export flight declaration to flarmcfg.txt file
  * @Author: eggert.ehmke@berlin.de
  */
int Flarm::exportDeclaration(FRTaskDeclaration* decl, QList<Waypoint*>* wpList)
{
    //TODO: reuse for upload
    qDebug ("Flarm::exportDeclaration");

    QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save File"),
                            QDir::homePath() + QDir::separator() + "flarmcfg.txt",
                            tr("FlarmCfg (flarmcfg.txt)"), 0);

    QFile file (fileName);
    file.setPermissions (QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
    if(!file.open(QIODevice::WriteOnly)) {
      qDebug()<<"Error opening the file";
      return FR_ERROR;
    }

    QTextStream stream(&file);

    int result = sendStreamData (stream, decl, wpList, true);
    file.close();
    return result;
}

void Flarm::sendStreamComment (QTextStream& stream, const QString& comment, bool isFile) {
  if (isFile)
    stream << "// " << comment << ENDL;
}

void Flarm::sendStreamData (QTextStream& stream, const QString& sentence, bool isFile) {
  if (isFile)
    stream << sentence << ENDL;
  else {
    ushort cs = calcCheckSum (sentence.length(), sentence);
    // qDebug () << "cs: " << cs << endl;
    QString str = sentence + "*";
    QString ccr = QString ("%1").arg (cs, 2, 16, QChar('0'));
    stream << str << ccr << ENDL;
  }
}

int Flarm::sendStreamData (QTextStream& stream, FRTaskDeclaration* decl, QList<Waypoint*>* wpList, bool isFile) {

    QDateTime now = QDateTime::currentDateTime();
    QString timestamp = now.toString ();

    sendStreamComment (stream, "FLARM configuration file has been created by KFlog", isFile);
    sendStreamComment (stream, timestamp, isFile);

    sendStreamComment (stream, "activated competition mode", isFile);
    sendStreamData (stream, "$PFLAC,S,CFLAGS,2", isFile);

    sendStreamComment (stream, "deaktivated Stealth mode", isFile);
    sendStreamData (stream, "$PFLAC,S,PRIV,0", isFile);

    sendStreamComment (stream, "aircraft type;  1 = glider", isFile);
    sendStreamData (stream, "$PFLAC,S,ACFT,1", isFile);

    sendStreamComment (stream, "Pilot name", isFile);
    sendStreamData (stream, "$PFLAC,S,PILOT," + decl->pilotA, isFile);

    if (!decl->pilotB.isEmpty()) {
      sendStreamComment (stream, "Copilot name", isFile);
      sendStreamData (stream, "$PFLAC,S,COPIL," + decl->pilotB, isFile);
    }

    sendStreamComment (stream, "Glider type", isFile);
    sendStreamData (stream, "$PFLAC,S,GLIDERTYPE," + decl->gliderType, isFile);

    sendStreamComment (stream, "Aircraft registration", isFile);
    sendStreamData (stream, "$PFLAC,S,GLIDERID," + decl->gliderID, isFile);

    sendStreamComment (stream, "Competition ID", isFile);
    sendStreamData (stream, "$PFLAC,S,COMPID," + decl->compID, isFile);

    sendStreamComment (stream, "Competition Class", isFile);
    sendStreamData (stream, "$PFLAC,S,COMPCLASS," + decl->compClass, isFile);

    //TODO: make configurable?
    sendStreamComment (stream, "Logger interval", isFile);
    sendStreamData (stream, "$PFLAC,S,LOGINT,4", isFile);

    //TODO: use task name?
    sendStreamComment (stream, "Task declaration", isFile);
    sendStreamData (stream, "$PFLAC,S,NEWTASK,new task", isFile);

    int wpCnt = 0;
    Waypoint *wp;

    foreach(wp, *wpList)
    {
        // should never happen
        if (wpCnt >= (int)_capabilities.maxNrWaypointsPerTask)
            break;

        // ignore take off and landing
        //if (wp->type == FlightTask::TakeOff || wp->type == FlightTask::Landing)
        //    continue;

        //int index = findWaypoint (wp);
        // qDebug ("wp: %s", wp->name.toLatin1().constData());
        sendStreamData (stream, "$PFLAC,S,ADDWP," + lat2flarm(wp->origP.lat()) + "," + lon2flarm(wp->origP.lon()) + "," + wp->name, isFile);
    }

    return FR_OK;
}

int Flarm::readDatabase()
{
  return FR_NOTSUPPORTED;
}

/**
  * read the tasks from the Flarm device
  */
int Flarm::readTasks(QList<FlightTask*> * /*tasks*/)
{
  qDebug ("Flarm::readTasks");

  return FR_NOTSUPPORTED;
}

/**
  * write the tasks to the flarm device
  */
int Flarm::writeTasks(QList<FlightTask*>* /*tasks*/)
{
  qDebug ("Flarm::writeTasks");

  return FR_NOTSUPPORTED;
}

/**
  * read the waypoints from the flarm device
  */
int Flarm::readWaypoints(QList<Waypoint*>* /*wpList*/)
{
  qDebug ("Flarm::readWaypoints");

  return FR_NOTSUPPORTED;
}

/**
  * write the waypoints to the flarm recorder
  * read the da4 buffer
  * write waypoints
  * write the buffer back to recorder
  */
int Flarm::writeWaypoints(QList<Waypoint*>* /*wpList*/)
{
  qDebug( "Flarm::writeWaypoints" );

  return FR_NOTSUPPORTED;
}

/**
 * Opens the recorder for other communication.
 */
int Flarm::openRecorder(const QString& /*URL*/)
{
  return FR_NOTSUPPORTED;
}