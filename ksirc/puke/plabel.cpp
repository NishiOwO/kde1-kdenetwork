#include <qmovie.h> 
#include "plabel.h"

PWidget *createWidget(widgetId *pwi, PWidget *parent)
{
  PLabel *pw = new PLabel(parent);
  QLabel *le;
  if(parent != 0)
    le = new QLabel(parent->widget());
  else
    le = new QLabel();
  pw->setWidget(le);
  pw->setWidgetId(pwi);
  return pw;
}


PLabel::PLabel(PWidget *parent)
  : PFrame(parent)
{
  //  debug("PLabel PLabel called");
  label = 0;
  setWidget(label);
}

PLabel::~PLabel()
{
  //  debug("PLabel: in destructor"); 
  delete widget();     // Delete the frame
  label=0;          // Set it to 0
  setWidget(label); // Now set all widget() calls to 0.
}

void PLabel::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_LABEL_SETTEXT:
    if(!checkWidget())
      return;

    widget()->setText(pm->cArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg[0] = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LABEL_SETPIXMAP:
    if(!checkWidget())
       return;

    widget()->setPixmap(QPixmap(pm->cArg));
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg[0] = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LABEL_SETMOVIE:
    if(!checkWidget())
      return;

    widget()->setMovie(QMovie(pm->cArg));
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg[0] = 0;
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LABEL_SETALIGNMENT:
    if(!checkWidget())
      return;

    widget()->setAlignment(pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = pm->iWinId;
    pmRet.iArg = 0;
    pmRet.cArg[0] = 0;
    emit outputMessage(fd, &pmRet);
    break;
  default:
    PFrame::messageHandler(fd, pm);
  }
}

void PLabel::setWidget(QLabel *_l)
{
  label = _l;
  PWidget::setWidget(_l);

}


QLabel *PLabel::widget()
{
  return label;
}

bool PLabel::checkWidget(){
  if(widget() == 0){
    debug("PLabel: No Widget set");
    return FALSE;
  }
  return TRUE;
}

