#include "pwidget.h"
#include "commands.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <qpixmap.h>
#include <qevent.h>

PWidget::PWidget(PObject *)
  : PObject()
{
  //  debug("PWidget constructor called");

  w = 0;
  setWidget(0);

  eventList[0] = &eventNone;
  eventList[1] = &eventTimer;
  eventList[2] = &eventMouse;
  eventList[3] = &eventMouse;
  eventList[4] = &eventMouse;
  eventList[5] = &eventMouse;
  eventList[6] = &eventKey;
  eventList[7] = &eventKey;
  eventList[8] = &eventFocus;
  eventList[9] = &eventFocus;
  eventList[10] = &eventFocus;
  eventList[11] = &eventFocus;
  eventList[12] = &eventPaint;
  eventList[13] = &eventMove;
  eventList[14] = &eventResize;
  eventList[15] = &eventNone;
  eventList[16] = &eventNone;
  eventList[17] = &eventNone;
  eventList[18] = &eventNone;
  eventList[19] = &eventNone;

  // Connect slots as needed

}

PWidget::~PWidget()
{
  //  debug("PWidget: in destructor");
  delete widget();
  w = 0;
  setWidget(0);
}

PObject *PWidget::createWidget(CreateArgs &ca)
{
  PWidget *pw = new PWidget();
  QWidget *tw;
  if(ca.fetchedObj != 0 && ca.fetchedObj->inherits("QWidget") == TRUE)
    tw = (QWidget *) ca.fetchedObj;
  else if(ca.parent != 0 && ca.parent->widget()->isWidgetType() == TRUE)
    tw = new QWidget((QWidget *) ca.parent->widget());
  else
    tw = new QWidget();
  pw->setWidget(tw);
  pw->setWidgetId(ca.pwI);
  pw->setPukeController(ca.pc);
  return pw;
}

void PWidget::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_WIDGET_SHOW:
    widget()->show();
    pmRet.iCommand = PUKE_WIDGET_SHOW_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_WIDGET_HIDE:
    widget()->hide();
    pmRet.iCommand = PUKE_WIDGET_HIDE_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;    
  case PUKE_WIDGET_REPAINT:
    widget()->repaint(pm->iArg);
    pmRet.iCommand = PUKE_WIDGET_REPAINT_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_WIDGET_RESIZE:
    {
      unsigned short int *size;
      size = (unsigned short int *) &pm->iArg;
      //      debug("Resizing to: %d => %d %d", pm->iArg, size[0], size[1]);
      widget()->resize(size[0], size[1]);
      size[0] = (short) widget()->height();
      size[1] = (short) widget()->width();
      pmRet.iCommand = PUKE_WIDGET_RESIZE_ACK;
      pmRet.iWinId = pm->iWinId;      
      pmRet.iArg = pm->iArg;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
    }
    break;
  case PUKE_WIDGET_MOVE:
    {
      unsigned short int *pos;
      pos = (unsigned short int *) &pm->iArg;
      //      debug("Moving to: %d => %d %d", pm->iArg, pos[0], pos[1]);
      widget()->move(pos[0], pos[1]);
      pos[0] = (short) widget()->x();
      pos[1] = (short) widget()->y();
      pmRet.iCommand = PUKE_WIDGET_MOVE_ACK;
      pmRet.iWinId = pm->iWinId;      
      pmRet.iArg = pm->iArg;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
    }
    break;
  case PUKE_WIDGET_SETMINSIZE:
    {
      unsigned short int *pos;
      pos = (unsigned short int *) &pm->iArg;
      widget()->setMinimumSize(pos[0], pos[1]);

      pos[0] = (short) widget()->width();
      pos[1] = (short) widget()->height();
      pmRet.iCommand = PUKE_WIDGET_SETMINSIZE_ACK;
      pmRet.iWinId = pm->iWinId;      
      pmRet.iArg = pm->iArg;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
    }
    break;
  case PUKE_WIDGET_SETMAXSIZE:
      unsigned short int *pos;
      pos = (unsigned short int *) &pm->iArg;
      widget()->setMaximumSize(pos[0], pos[1]);

      pos[0] = (short) widget()->width();
      pos[1] = (short) widget()->height();
      pmRet.iCommand = -pm->iCommand;
      pmRet.iWinId = pm->iWinId;      
      pmRet.iArg = pm->iArg;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
      break;
  case PUKE_WIDGET_SETCAPTION:
    widget()->setCaption(pm->cArg);
    pmRet.iCommand = PUKE_WIDGET_SETCAPTION_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.iTextSize = strlen(widget()->caption());
    pmRet.cArg = strdup(widget()->caption());
    emit outputMessage(fd, &pmRet);
    free(pmRet.cArg);
    break;
  case PUKE_WIDGET_GET_BACKGROUND_COLOUR:
    {
      pmRet.cArg = new char[15];
      pmRet.iTextSize = 15;
      QColor back = widget()->backgroundColor();
      debug("Colour are: %d %d %d", back.red(), back.green(), back.blue());
      sprintf(pmRet.cArg, "%d,%d,%d", back.red(), back.green(), back.blue());
      
      pmRet.iCommand = -pm->iCommand;
      pmRet.iWinId = pm->iWinId;      
      pmRet.iArg = 0;
      emit outputMessage(fd, &pmRet);
      delete pmRet.cArg;
      break;
    }
  case PUKE_WIDGET_SET_BACKGROUND_PIXMAP:
    widget()->setBackgroundPixmap(QPixmap(pm->cArg));
    debug("Current background Mode: %d", widget()->backgroundMode());

    pmRet.iCommand = -pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_WIDGET_SET_BACKGROUND_MODE:
    widget()->setBackgroundMode((QWidget::BackgroundMode) pm->iArg);

    pmRet.iCommand = -pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = (int) widget()->backgroundMode();
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;

  case PUKE_WIDGET_SET_BACKGROUND_COLOUR:
    {
      int *pos;
      pos = (int *) pm->cArg;
      QColor bg(pos[0], pos[1], pos[2]);
      QColorGroup cg = QColorGroup(widget()->colorGroup().foreground(),
                                   bg,
                                   widget()->colorGroup().light(),
                                   widget()->colorGroup().dark(),
                                   widget()->colorGroup().mid(),
                                   widget()->colorGroup().text(),
                                   bg);
      widget()->setPalette(QPalette(cg,cg,cg));

      pmRet.iCommand = -pm->iCommand;
      pmRet.iWinId = pm->iWinId;      
      pmRet.iArg = 0;
      pmRet.cArg = 0;
      emit outputMessage(fd, &pmRet);
      break;
    }

  case PUKE_WIDGET_SET_ENABLED:
    widget()->setEnabled((bool) pm->iArg);
    pmRet.iCommand = PUKE_WIDGET_SET_ENABLED_ACK;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_WIDGET_RECREATE:
  {
    QWidget *nparent = 0x0;
    if(pm->iArg != 0x0){
      widgetId wiWidget;
      wiWidget.fd = fd;
      wiWidget.iWinId = pm->iArg;
      nparent = controller()->id2pwidget(&wiWidget)->widget();
    }
    if(pm->iTextSize != 3*sizeof(int)){
       throw(errorCommandFailed(pm->iCommand, pm->iArg));
    }
    int *point_show = (int *) pm->cArg;

    warning("Recreate: %d %d %d", point_show[0], point_show[1], point_show[3]);
    
    widget()->recreate(nparent, (WFlags) 0, QPoint(point_show[0], point_show[1]), point_show[3]);

   pmRet.iCommand = PUKE_WIDGET_RECREATE_ACK;
   pmRet.iWinId = pm->iWinId;
   pmRet.iArg = 0;
   pmRet.cArg = 0;
   emit outputMessage(fd, &pmRet);
   break; 
  }
  default:
    PObject::messageHandler(fd, pm);
  }
}

void PWidget::setWidget(QWidget *_w)
{
  //  debug("PWidget setwidget called");
  w = _w;
  if(w != 0){
    widget()->installEventFilter(this);
  }
  PObject::setWidget(_w);
}

QWidget *PWidget::widget()
{
  //  debug("PWidget widget called");
  return w;
}

// PWidget specific
bool PWidget::eventFilter(QObject *o, QEvent *e)
{
//  debug("PWidget(%d): Got event: %d", widget(), e->type());
//  fprintf(stderr, "Blah\n");
  if(e->type() < 20 && e->type() >= 0){
    (this->*(eventList[e->type()]))(o,e);
  }
  else{
    PukeMessage pm;
    widgetId wI;
    
    wI = widgetIden();
    pm.iCommand = PUKE_EVENT_UNKOWN;
    pm.iWinId = wI.iWinId;
    pm.iArg = e->type();
    pm.cArg = 0;
    emit outputMessage(wI.fd, &pm);

  }

  return false;
}

void PWidget::eventNone(QObject *, QEvent *e)  
{
  PukeMessage pm;
  widgetId wI;

  //  debug("PWidget: eventNone");
  
  wI = widgetIden();
  pm.iCommand = - e->type() - 1020; // 1030 offset for events
  pm.iWinId = wI.iWinId;
  pm.iArg = 0;
  pm.cArg = 0;

  emit outputMessage(wI.fd, &pm);
  
}

void PWidget::eventTimer(QObject *, QEvent *e)  
{
  PukeMessage pm;
  widgetId wI;

  QTimerEvent *et = Q_TIMER_EVENT(e);
  
  wI = widgetIden();
  pm.iCommand = PUKE_WIDGET_EVENT_TIMER;
  pm.iWinId = wI.iWinId;
  pm.iArg = et->timerId();
  pm.cArg = 0;

  emit outputMessage(wI.fd, &pm);
  
}

void PWidget::eventMouse(QObject *, QEvent *e)  
{
  PukeMessage pm;
  widgetId wI;

  QMouseEvent *me = Q_MOUSE_EVENT(e);
  
  wI = widgetIden();
  pm.iCommand = - e->type() - 1020; // 1020 offset for events
  pm.iWinId = wI.iWinId;
  pm.iArg = 0;

  // special cArg handling
  pm.iTextSize = 4*sizeof(int);
  int *icArg = new int[4];
  icArg[0] = me->x();
  icArg[1] = me->y();
  icArg[2] = me->button();
  icArg[3] = me->state();
  pm.cArg = (char *) icArg;

  emit outputMessage(wI.fd, &pm);
  
  delete[] icArg;
  
}

void PWidget::eventKey(QObject *, QEvent *e)  
{
  PukeMessage pm;
  widgetId wI;

  QKeyEvent *ke = Q_KEY_EVENT(e);
  
  wI = widgetIden();
  pm.iCommand = - e->type() - 1020; // 1020 offset for events
  pm.iWinId = wI.iWinId;
  pm.iArg = 0;

  // special cArg handling
  pm.iTextSize = 3*sizeof(int);
  int *icArg = new int[3];
  icArg[0] = ke->key();
  icArg[1] = ke->ascii();
  icArg[2] = ke->state();
  pm.cArg = (char *) icArg;

  emit outputMessage(wI.fd, &pm);

  delete[] icArg;
  
}

void PWidget::eventFocus(QObject *, QEvent *e)  
{
  PukeMessage pm;
  widgetId wI;

  //  debug("PWidget: eventFocus");

  QFocusEvent *fe = Q_FOCUS_EVENT(e);
  
  wI = widgetIden();
  pm.iCommand = - e->type() - 1020; // 1020 offset for events
  pm.iWinId = wI.iWinId;
  pm.iArg = 0;

  pm.cArg = new char[2];
  pm.iTextSize = 2*sizeof(char);
  pm.cArg[0] = fe->gotFocus();
  pm.cArg[1] = fe->lostFocus();

  emit outputMessage(wI.fd, &pm);

  delete[] pm.cArg;
}

void PWidget::eventPaint(QObject *, QEvent *e)  
{
  PukeMessage pm;
  widgetId wI;

  wI = widgetIden();
  pm.iCommand = - e->type() - 1020; // 1020 offset for events
  pm.iWinId = wI.iWinId;
  pm.iArg = 0;
  pm.cArg = 0;

  emit outputMessage(wI.fd, &pm);
  
}

void PWidget::eventMove(QObject *, QEvent *e)  
{
  PukeMessage pm;
  widgetId wI;

  QMoveEvent *me = Q_MOVE_EVENT(e);
  
  wI = widgetIden();
  pm.iCommand = - e->type() - 1020; // 1020 offset for events
  pm.iWinId = wI.iWinId;
  pm.iArg = 0;

  // special cArg handling
  pm.iTextSize = 4*sizeof(int);
  int *icArg = new int[4];
  icArg[0] = me->pos().x();
  icArg[1] = me->pos().y();
  icArg[2] = me->oldPos().x();
  icArg[3] = me->oldPos().y();
  pm.cArg = (char *) icArg;

  emit outputMessage(wI.fd, &pm);

  delete[] icArg;
  
}

void PWidget::eventResize(QObject *, QEvent *e)  
{
  PukeMessage pm;
  widgetId wI;

  QResizeEvent *re = Q_RESIZE_EVENT(e);
  
  wI = widgetIden();
  pm.iCommand = - e->type() - 1020; // 1020 offset for events
  pm.iWinId = wI.iWinId;
  pm.iArg = 0;

  // special cArg handling
  pm.iTextSize = 4*sizeof(int);
  int *icArg = new int[4];
  icArg[0] = re->size().height();
  icArg[1] = re->size().width();
  icArg[2] = re->oldSize().height();
  icArg[3] = re->oldSize().width();
  pm.cArg = (char *) icArg;

  emit outputMessage(wI.fd, &pm);

  delete[] icArg;
  
}

