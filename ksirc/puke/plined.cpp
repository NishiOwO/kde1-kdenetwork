#include "plined.h"
#include <stdio.h>

PWidget *createWidget(widgetId *pwi, PWidget *parent)
{
  PLineEdit *pw = new PLineEdit(parent);
  QLineEdit *le;
  if(parent != 0)
    le = new QLineEdit(parent->widget());
  else
    le = new QLineEdit();
  pw->setWidget(le);
  pw->setWidgetId(pwi);
  return pw;
}


PLineEdit::PLineEdit(PWidget *parent)
  : PWidget(parent)
{
  debug("PLineEdit PLineEdit called");
  lineedit = 0;
  setWidget(lineedit);
}

PLineEdit::~PLineEdit()
{
  debug("PLineEdit: in destructor"); 
  delete widget();     // Delete the frame
  lineedit=0;          // Set it to 0
  setWidget(lineedit); // Now set all widget() calls to 0.
}

void PLineEdit::messageHandler(int fd, PukeMessage *pm)
{
  PukeMessage pmRet;
  switch(pm->iCommand){
  case PUKE_LINED_SET_MAXLENGTH:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    widget()->setMaxLength(pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = - pm->iWinId;
    pmRet.iArg = widget()->maxLength();
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LINED_SET_ECHOMODE:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    widget()->setEchoMode((QLineEdit::EchoMode) pm->iArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = - pm->iWinId;
    pmRet.iArg = widget()->echoMode();
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LINED_SET_TEXT:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    pm->cArg[49] = 0;
    debug("PukeLine Edit: Got: %s", pm->cArg);
    widget()->setText(pm->cArg);
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = - pm->iWinId;
    pmRet.iArg = 0;
    strcpy(pmRet.cArg, widget()->text());
    emit outputMessage(fd, &pmRet);
    break;
  case PUKE_LINED_GET_TEXT:
    if(widget() == 0){
      debug("PLineEdit: No Widget set");
      return;
    }
    pmRet.iCommand = - pm->iCommand;
    pmRet.iWinId = - pm->iWinId;
    pmRet.iArg = 0;
    strcpy(pmRet.cArg, widget()->text());
    emit outputMessage(fd, &pmRet);
    break;
  default:
    PWidget::messageHandler(fd, pm);
  }
}

void PLineEdit::setWidget(QLineEdit *_le)
{
  lineedit = _le;
  if(lineedit != 0){
    connect(lineedit, SIGNAL(textChanged(const char *)),
	    this, SLOT(updateText(const char *)));
    connect(lineedit, SIGNAL(returnPressed()),
	    this, SLOT(returnPress()));
  }
  PWidget::setWidget(_le);

}


QLineEdit *PLineEdit::widget()
{
  return lineedit;
}

void PLineEdit::updateText(const char *text){
  PukeMessage pmRet;

  pmRet.iCommand = PUKE_LINED_GET_TEXT_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = 0;
  strncpy(pmRet.cArg, text, 50);
  emit outputMessage(widgetIden().fd, &pmRet);
}

void PLineEdit::returnPress() {
  PukeMessage pmRet;

  pmRet.iCommand = PUKE_LINED_RETURN_PRESSED_ACK;
  pmRet.iWinId = widgetIden().iWinId;
  pmRet.iArg = 0;
  pmRet.cArg[0] = 0;
  emit outputMessage(widgetIden().fd, &pmRet);
}
