/***********************************************************************
**
**   evaluationdialog.cpp
**
**   This file is part of KFLog.
**
************************************************************************
**
**   Copyright (c):  2000 by Heiner Lamprecht, Florian Ehinger
**                :  2008 by Constantijn Neeteson
**                :  2011 by Axel Pauli
**
**   This file is distributed under the terms of the General Public
**   License. See the file COPYING for more information.
**
**   $Id$
**
***********************************************************************/

#include <QtGui>

#include "evaluationdialog.h"
#include "evaluationframe.h"
#include "evaluationview.h"
#include "flight.h"
#include "mainwindow.h"
#include "mapcontents.h"

extern MapContents* _globalMapContents;
extern QSettings    _settings;

EvaluationDialog::EvaluationDialog( QWidget *parent ) : QWidget( parent )
{
  setWindowTitle( tr("Flight Evaluation:") );

  // movable text display
  QSplitter* textSplitter = new QSplitter( Qt::Vertical, this );

  // upper diagram widget
  evalFrame = new EvaluationFrame( textSplitter, this );

  connect( this, SIGNAL(flightChanged()),
           evalFrame, SLOT(slotShowFlight()));

  connect( this, SIGNAL(textChanged(QString)),
           evalFrame, SLOT(slotUpdateCursorText(QString)));

  // lower text display
  textDisplay = new QTextBrowser( textSplitter );
  textDisplay->setMinimumHeight( 5 );

  QVBoxLayout* layout = new QVBoxLayout( this );
  layout->setMargin( 5 );
  layout->setSpacing( 1 );
  layout->addWidget(textSplitter);

  // Setting default-values for the splitter
/*  typedef QValueList<int> testList;
  testList list;
  list.append(100);
  list.append(60);
  textSplitter->setSizes(list);
*/

  if( _settings.value("/EvaluationDialog/FirstOpen", true).toBool() )
    {
      // On first call we need a reasonable size of the widget. All other
      // is later on handled by the dock widget.
      _settings.setValue( "/EvaluationDialog/FirstOpen", false );
      resize(800, 600);
    }
}

EvaluationDialog::~EvaluationDialog()
{
}

void EvaluationDialog::hideEvent( QHideEvent* event )
{
  Q_UNUSED(event)

  emit windowHidden();
}

void EvaluationDialog::updateText(int index1, int index2, bool updateAll)
{

// warning("EvaluationDialog::updateText(%d, %d, %d)",index1,index2,updateAll);

  QString htmlText;
  QString text;
  FlightPoint p1;
  FlightPoint p2;

  if ( ! flight )
  {
    emit textChanged(htmlText);
    return;
  }

  switch(flight->getObjectType())
  {

  case BaseMapElement::Flight:

    p1 = flight->getPoint(index1);
    p2 = flight->getPoint(index2);

    htmlText = (QString)"<TABLE BORDER=0 CELLPADDING=0 CELLSPACING=0>"+
              "<TR><TD WIDTH=100 ALIGN=center><FONT COLOR=#00bb00><B>" +
              printTime(p1.time, true) + "</B></TD><TD WIDTH=70 ALIGN=right>";
    text.sprintf("%5d m", p1.height);
    htmlText += text + "</TD><TD WIDTH=80 ALIGN=right>";
    text.sprintf("%.1f m/s", getVario(p1));
    htmlText += text + "</TD><TD WIDTH=100 ALIGN=right>";
    text.sprintf("%.1f km/h", getSpeed(p1));
    htmlText += text + "</TD>";

    htmlText += (QString) "<TD WIDTH=100 ALIGN=center><FONT COLOR=#bb0000><B>" +
              printTime(p2.time, true) + "</B></TD><TD WIDTH=70 ALIGN=right>";
    text.sprintf("%5d m", p2.height);
    htmlText += text + "</TD><TD WIDTH=80 ALIGN=right>";
    text.sprintf("%.1f m/s", getVario(p2));
    htmlText += text + "</TD><TD WIDTH=100 ALIGN=right>";
    text.sprintf("%.1f km/h", getSpeed(p2));
    htmlText += text + "</TD></TR>";

    emit textChanged(htmlText);
    htmlText = QString::null;

    if(updateAll)
      {
        QStringList erg = flight->getFlightValues(index1, index2);

        htmlText = (QString) "<TABLE BORDER=0 CELLPADDING=0 CELLSPACING=0>\
                              <TR><TD width='50'><B>" + tr("Circling") + "</B></TD> \
                              <TD align='right'><I>" + tr("Time") + "</I></TD> \
                              <TD align='right'><I>" + tr("Vario") + "</I></TD> \
                              <TD align='right'><I>" + tr("Alt. Gain") + "</I></TD> \
                              <TD align='right'><I>" + tr("Alt. Loss") + "</I></TD></TR> \
                              <TD align='right'><I>" + tr("Alt. Netto") + "</I></TD></TR>";
        htmlText += (QString) "<TR><TD>total:</TD> \
                           <TD ALIGN=right WIDTH=130>" + erg.at(3) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(7) + \
                          "</TD><TD ALIGN=right>" + erg.at(11) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(15) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(19) + \
                          "</TD></TR>";
        htmlText += (QString) "<TR><TD>right:</TD> \
                           <TD ALIGN=right WIDTH=130>" + erg.at(0) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(4) + \
                          "</TD><TD ALIGN=right>" + erg.at(8) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(12) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(16) + \
                          "</TD></TR>";
        htmlText += (QString) "<TR><TD>left:</TD> \
                           <TD ALIGN=right WIDTH=130>" + erg.at(1) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(5) + \
                          "</TD><TD ALIGN=right>" + erg.at(9) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(13) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(17) + \
                          "</TD></TR>";
        htmlText += (QString) "<TR><TD>mixed:</TD> \
                           <TD ALIGN=right WIDTH=130>" + erg.at(2) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(6) + \
                          "</TD><TD ALIGN=right>" + erg.at(10) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(14) + \
                          "</TD><TD ALIGN=right>" + (QString)erg.at(18) + \
                          "</TD></TR>";

        htmlText += (QString) "<TR><TD colspan='7'></TD></TR> \
                                        <TR><TD><B>" + tr("Straight") + "</B></TD> \
                                        <TD align='right'><I>" + tr("Time") + "</I></TD> \
                                        <TD align='right'><I>" + tr("tot. Distance") + "</I></TD> \
                                        <TD align='right'><I>" + tr("Alt. Gain") + "</I></TD> \
                                        <TD align='right'><I>" + tr("Alt. Loss") + "</I></TD> \
                                        <TD align='right'><I>" + tr("Alt. Netto") + "</I></TD> \
                                        <TD align='right'><I>" + tr("Speed") + "</I></TD> \
                                        <TD align='right'><I>" + tr("L/D ratio") + "</I></TD></TR>";
        htmlText += (QString) "<TR><TD></TD> \
                                        <TD ALIGN=right>" + erg.at(26) + "</TD> \
                                        <TD ALIGN=right>" + erg.at(25) + "</TD> \
                                        <TD ALIGN=right>" + erg.at(22) + "</TD> \
                                        <TD ALIGN=right>" + erg.at(23) + "</TD> \
                                        <TD ALIGN=right>" + erg.at(24) + "</TD> \
                                        <TD ALIGN=right>" + erg.at(21) + "</TD> \
                                        <TD ALIGN=right>" + erg.at(20) + "</TD> \
                                        </TR>";

        htmlText += (QString) "<TR><TD colspan='7'></TD></TR>" + \
                                        "<TR><TD><B>" + tr("Total") + "</B></TD> \
                                        <TD ALIGN=right>" + erg.at(27) + "</TD> \
                                        <TD></TD> \
                                        <TD ALIGN=right>" + erg.at(28) + "</TD> \
                                        <TD ALIGN=right>" + erg.at(29) + "</TD> \
                                        </TR></TABLE><BR><BR><HR><BR><BR>";

        QList<statePoint*> state_list;
        QString text = "";
        state_list = flight->getFlightStates(index1, index2);
        htmlText += (QString) "<TABLE border='0' cellpadding='0' cellspacing='0'><TR><TD><B>" + tr("Flight sections") + "</B></TR> \
                                <TR> \
                                <TD align='left'><I>" + tr("Type") + "</I></TD> \
                                <TD align='center'><I>" + tr("Start") + "</I></TD> \
                                <TD align='center'><I>" + tr("End") + "</I></TD> \
                                <TD align='center'><I>" + tr("Time") + "</I></TD> \
                                <TD align='right'><I>" + tr("Alt. Gain") + "</I></TD> \
                                <TD align='right'><I>" + tr("Alt. Loss") + "</I></TD> \
                                <TD align='right'><I>" + tr("Alt. Netto") + "</I></TD> \
                                <TD align='right'><I>" + tr("Distance") + "</I></TD> \
                                <TD align='center'><I>" + tr("Speed") + "</I></TD> \
                                <TD align='right'><I>" + tr("L/D") + "</I></TD> \
                                <TD align='center'><I>" + tr("Vario") + "</I></TD></TR>";

        for(int n = 0; n<state_list.count(); n++)
        {
          if(n%10==0 && n!=0)
            htmlText += (QString) "<TR> \
                        <TD align='left'><I>" + tr("Type") + "</I></TD> \
                        <TD align='center'><I>" + tr("Start") + "</I></TD> \
                        <TD align='center'><I>" + tr("End") + "</I></TD> \
                        <TD align='center'><I>" + tr("Time") + "</I></TD> \
                        <TD align='right'><I>" + tr("Alt. Gain") + "</I></TD> \
                        <TD align='right'><I>" + tr("Alt. Loss") + "</I></TD> \
                        <TD align='right'><I>" + tr("Alt. Netto") + "</I></TD> \
                        <TD align='right'><I>" + tr("Distance") + "</I></TD> \
                        <TD align='center'><I>" + tr("Speed") + "</I></TD> \
                        <TD align='right'><I>" + tr("L/D") + "</I></TD> \
                        <TD align='center'><I>" + tr("Vario") + "</I></TD></TR>";

          switch(state_list.at(n)->f_state)
          {
            case Flight::RightTurn: text.sprintf("%s%.1f %s", "Circling (", state_list.at(n)->circles, " right)"); break;
            case Flight::LeftTurn: text.sprintf("%s%.1f %s", "Circling (", state_list.at(n)->circles, "left)"); break;
            case Flight::MixedTurn: text.sprintf("%s%.1f %s", "Circling (", state_list.at(n)->circles, "mixed)"); break;
            case Flight::Straight: text = (QString)"Straight"; break;
          }
          //text.sprintf("%i", state_list.at(n)->f_state);
          htmlText += (QString) "<TR><TD align='left'>" + text + "</TD>";
          text.sprintf("%s", (const char*)printTime(state_list.at(n)->start_time, true));
          htmlText += (QString) "<TD align='right'>" + text + "</TD>";
          text.sprintf("%s", (const char*)printTime(state_list.at(n)->end_time, true));
          htmlText += (QString) "<TD align='right'>" + text + "</TD>";
          text.sprintf("%s", (const char*)printTime(state_list.at(n)->duration, true, true, true));
          htmlText += (QString) "<TD align='right'>" + text + "</TD>";
          text.sprintf("%i", state_list.at(n)->dH_pos);
          htmlText += (QString) "<TD align='right'>" + text + " m</TD>";
          text.sprintf("%i", state_list.at(n)->dH_neg);
          htmlText += (QString) "<TD align='right'>" + text + " m</TD>";
          text.sprintf("%i", (state_list.at(n)->dH_pos+state_list.at(n)->dH_neg));
          htmlText += (QString) "<TD align='right'>" + text + " m</TD>";
          text.sprintf("%.1f", state_list.at(n)->distance);
          htmlText += (QString) "<TD align='right'>" + text + " km</TD>";
          if(state_list.at(n)->f_state==Flight::Straight)
            text.sprintf("%.1f %s", state_list.at(n)->speed, "km/h");
          else
            text = (QString) "";
          htmlText += (QString) "<TD align='right'>" + text + "</TD>";
          if(state_list.at(n)->f_state==Flight::Straight)
          {
            if(abs((int) state_list.at(n)->L_D)>1000)
                text.sprintf("%s", "inf");
            else
                text.sprintf("%.0f", state_list.at(n)->L_D);
          }
          else
            text = (QString) "";
          htmlText += (QString) "<TD align='right'>" + text + "</TD>";
          text.sprintf("%.1f", state_list.at(n)->vario);
          htmlText += (QString) "<TD align='right'>" + text + " m/s</TD>";
          htmlText += (QString) "</TR>";
          }

        htmlText += (QString) "</TABLE>";
      }


    emit showCursor(p1.projP,p2.projP);
    break;

  case BaseMapElement::Task:
    emit textChanged(QString::null);
    htmlText = "<DIV ALIGN=CENTER>" + tr("Cannot evaluate task") + "</DIV>";
    break;

  case BaseMapElement::FlightGroup:
    emit textChanged(QString::null);
    htmlText = "<DIV ALIGN=CENTER>" + tr("Cannot (yet) evaluate a flight group") + "</DIV>";
    break;

  default:
    break;
  }

 textDisplay->setHtml(htmlText);
}


void EvaluationDialog::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);

  slotShowFlightData();
}


void EvaluationDialog::slotShowFlightData()
{
  flight = dynamic_cast<Flight *> (_globalMapContents->getFlight());

  if( flight != static_cast<Flight *> (0) )
    {
      if( flight->getObjectType() == BaseMapElement::Flight )
        {
          setWindowTitle( tr( "Flight Evaluation:" ) +
                          flight->getPilot() + "  " +
                          flight->getDate().toString() );
        }
      else
        {
          setCaption( tr( "Flight Evaluation:" ) );
        }

      // set defaults
      updateText( 0, flight->getRouteLength() - 1, true );

      emit flightChanged();
    }
  else
    {
      updateText( 0, 0, false );
    }
}

/** No descriptions */
Flight* EvaluationDialog::getFlight()
{
  if( flight && flight->getObjectType() == BaseMapElement::Flight )
    {
      return flight;
    }
  else
    {
      return 0;
    }
}

/** No descriptions */
void EvaluationDialog::slotShowFlightPoint(const QPoint&, const FlightPoint& fp)
{
  emit(showFlightPoint(&fp));
}

/** No descriptions */
void EvaluationDialog::slotRemoveFlightPoint()
{
  emit(showFlightPoint(0));
}
