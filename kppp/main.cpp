/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id$
 * 
 *            Copyright (C) 1997 Bernd Johannes Wuebben 
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <config.h>

#include <qfileinf.h>
#include <unistd.h>
#include <kmsgbox.h>
#include <stdio.h>
#include <locale.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef BSD
#include <stdlib.h>
#else
#include <getopt.h>
#endif


#include "acctselect.h"
#include "main.h"
#include "version.h"
#include "macros.h"
#include "pap.h"
#include "chap.h"
#include "docking.h"
#include "runtests.h"

#include <X11/Xlib.h>

XPPPWidget*	p_xppp;
KApplication*	app;
DockWidget*     dock_widget;
QString 	cmdl_account;
QPixmap *miniIcon = 0;

bool	have_cmdl_account;
bool 	pppd_has_died = false;
bool 	reconnect_on_disconnect = false;
bool 	modem_is_locked = false;
bool    quit_on_disconnect = false;

int totalbytes;

QString old_hostname;
QString local_ip_address;
QString remote_ip_address;

void terminatepppd();

void usage(char* progname){

  fprintf(stderr, "%s -- valid command line options:\n", progname);
  fprintf(stderr, " -h : describe command line options\n");
  fprintf(stderr, " -c account_name : connect to account account_name\n");
  fprintf(stderr, " -q : quit after end of connection\n");
  fprintf(stderr, " -r rule_file: check syntax of rule_file\n");
  exit(1);

}

void banner(char* progname){

  fprintf(stderr,"%s version " KPPPVERSION "\n",progname); 
  fprintf(stderr,"Copyright (c) 1997-1998 Bernd Johannes Wuebben ");
  fprintf(stderr,"wuebben@math.cornell.edu\n");
  fprintf(stderr,"Use -h for the list of valid of command line options.\n");
  exit(0);

}

extern "C" {

static int kppp_x_errhandler( Display *dpy, XErrorEvent *err )
{
    char errstr[256];

    /*
    if(gpppdata.pppdpid() >= 0) {
      kill(gpppdata.pppdpid(), SIGTERM);
    }

    p_xppp->stopAccounting();
    removedns();
    unlockdevice();*/

    XGetErrorText( dpy, err->error_code, errstr, 256 );
    fatal( "X Error: %s\n  Major opcode:  %d", errstr, err->request_code );
    return 0;
}


static int kppp_xio_errhandler( Display * ){


  if(gpppdata.get_xserver_exit_disconnect()){

    printf("X11 Error!");
    if(gpppdata.pppdpid() >= 0) {
      kill(gpppdata.pppdpid(), SIGTERM);
    }

    p_xppp->stopAccounting();
    removedns();
    unlockdevice();	
    return 0;
  }
  else{

    fatal( "%s: Fatal IO error: client killed", "kppp" );
    return 0;

  }
}

} /* extern "C" */                                  


void make_directories(){

  QDir dir;

  QString d = QDir::homeDirPath() + "/";
  d += "/.kde";

  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

  d += "/share";
  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

  d += "/apps";
  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

  d += "/kppp" ;

  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

  
  d += "/Rules/";

  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

  QString logdir = QDir::homeDirPath() + "/";
  logdir += ACCOUNTING_PATH "/Log";

  dir.setPath(logdir.data());
  if(!dir.exists()){
    dir.mkdir(logdir.data());
    chown(logdir.data(),getuid(),getgid());
    chmod(logdir.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }
}

#define MAX_NAME_LENGTH    64

int main( int argc, char **argv ) { 
  int c;
  opterr = 0;

  KApplication a(argc, argv,"kppp");

  // set portable locale for decimal point
  setlocale(LC_NUMERIC ,"C");

  while ((c = getopt(argc, argv, "c:hvr:q")) != -1){
    switch (c)
      {

      case '?':
	fprintf(stderr, "%s: unknown option \"%s\"\n", 
		argv[0], argv[optind-1]);
	usage(argv[0]);
	exit(1);	
      case 'c':
	{
	  // copy at most MAX_NAME_LENGTH bytes
	  char tmp[MAX_NAME_LENGTH];
	  strncpy(tmp, optarg, MAX_NAME_LENGTH-1);
	  
	  // terminate string
	  tmp[MAX_NAME_LENGTH-1] = 0; 
	  cmdl_account = tmp;
	  break;
	}
      case 'h':
	usage(argv[0]);
	break;
      case 'v':
	banner(argv[0]);
	break;
      case 'q':
	quit_on_disconnect = TRUE;
	break;
      case 'r':
	{
	  // drop root
	  setuid(getuid());

	  // we need a KAppliction for locales, create one
	  exit(RuleSet::checkRuleFile(optarg));
	}
      }
  }

  if(!cmdl_account.isEmpty()){

   have_cmdl_account = true;

#ifdef MY_DEBUG
    printf("cmdl_account:%s:\n",cmdl_account.data());
#endif

  }
  // load mini-icon
  miniIcon = new QPixmap(a.getMiniIcon());

  // make sure that nobody can read the password from the
  // config file
  QString configFile = a.localconfigdir() + "/" + a.appName() + "rc";
  if(access(configFile.data(), F_OK) == 0)
    chmod(configFile.data(), S_IRUSR | S_IWUSR);

  app = &a;

  make_directories();
  XPPPWidget xppp;
  p_xppp = &xppp;

  a.setMainWidget(&xppp);
  a.setTopWidget(&xppp);  

  // we really don't want to die accidentally, since that would leave the
  // modem connected. If you really really want to kill me you must send 
  // me a SIGKILL.
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  signal(SIGCHLD, dieppp);

  XSetErrorHandler( kppp_x_errhandler );
  XSetIOErrorHandler( kppp_xio_errhandler );
                                                 
  return a.exec();

}



XPPPWidget::XPPPWidget( QWidget *parent, const char *name )
  : QWidget(parent, name) 
{
  tabWindow = 0;

  bool config;
  config = gpppdata.open(app);

  // before doing anything else, run a few tests
  int result = runTests();
  if(result == TEST_CRITICAL)
    exit(4);

  connected = false;

  QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);

  fline1 = new QFrame(this,"line");
  fline1->setFrameStyle(QFrame::HLine |QFrame::Sunken);
  fline1->setFixedHeight(3);
  tl->addWidget(fline1);

  QGridLayout *l1 = new QGridLayout(3, 4);
  tl->addLayout(l1);
  l1->addColSpacing(0, 10);
  l1->addColSpacing(3, 10);
  l1->setColStretch(1, 3);
  l1->setColStretch(2, 4);

  label1 = new QLabel(this,"lable1");
  label1->setText(klocale->translate("Connect to: "));
  MIN_SIZE(label1);
  l1->addWidget(label1, 0, 1);

  connectto_c = new QComboBox(true,this, "connectto_c");
  connect(connectto_c, SIGNAL(activated(int)), 
	  SLOT(newdefaultaccount(int)));
  MIN_SIZE(connectto_c);
  l1->addWidget(connectto_c, 0, 2);

  ID_Label = new QLabel(this,"lableid");
  ID_Label->setText(klocale->translate("Login ID:"));
  MIN_SIZE(ID_Label);
  l1->addWidget(ID_Label, 1, 1);

  ID_Edit = new QLineEdit(this,"idedit");
  MIN_WIDTH(ID_Edit);
  FIXED_HEIGHT(ID_Edit);
  l1->addWidget(ID_Edit, 1, 2);
  connect(ID_Edit, SIGNAL(returnPressed()),
	  this, SLOT(enterPressedInID()));

  PW_Label = new QLabel(this,"lablepw");
  PW_Label->setText(klocale->translate("Password:"));
  MIN_SIZE(PW_Label);
  l1->addWidget(PW_Label, 2, 1);

  PW_Edit= new QLineEdit(this,"pwedit");
  PW_Edit->setEchoMode(QLineEdit::Password);
  MIN_WIDTH(PW_Edit);
  FIXED_HEIGHT(PW_Edit);
  l1->addWidget(PW_Edit, 2, 2);
  connect(PW_Edit, SIGNAL(returnPressed()),
	  this, SLOT(enterPressedInPW()));

  QHBoxLayout *l3 = new QHBoxLayout;
  tl->addSpacing(5);
  tl->addLayout(l3);
  tl->addSpacing(5);
  l3->addSpacing(10);
  log = new QCheckBox(klocale->translate("Show Log Window"), this,"log");
  connect(log, SIGNAL(toggled(bool)), 
	  this, SLOT(log_window_toggled(bool)));
  log->setChecked(gpppdata.get_show_log_window());
  MIN_SIZE(log);
  l3->addWidget(log);

  fline = new QFrame(this,"line");
  fline->setFrameStyle(QFrame::HLine |QFrame::Sunken);
  fline->setFixedHeight(3);
  tl->addWidget(fline);

  QHBoxLayout *l2 = new QHBoxLayout;
  tl->addLayout(l2);

  int minw = 0;
  quit_b = new QPushButton(klocale->translate("Quit"), this, "quit");
  connect( quit_b, SIGNAL(clicked()), SLOT(quitbutton()));
  MIN_HEIGHT(quit_b);
  if(quit_b->sizeHint().width() > minw)
    minw = quit_b->sizeHint().width();

  setup_b = new QPushButton(klocale->translate("Setup"), this, "setup");
  connect( setup_b, SIGNAL(clicked()), SLOT(expandbutton()));
  MIN_HEIGHT(setup_b);
  if(setup_b->sizeHint().width() > minw)
    minw = setup_b->sizeHint().width();

  if (!config) 
    setup_b->setEnabled(false);

  help_b = new QPushButton(klocale->translate("Help"), this, "help");
  connect( help_b, SIGNAL(clicked()), SLOT(helpbutton()));
  MIN_HEIGHT(help_b);
  if(help_b->sizeHint().width() > minw)
    minw = help_b->sizeHint().width();

  connect_b = new QPushButton(klocale->translate("Connect"), 
			      this, "connect_b");
  connect_b->setFocus();
  connect(connect_b, SIGNAL(clicked()), SLOT(connectbutton()));
  MIN_HEIGHT(connect_b);
  if(connect_b->sizeHint().width() > minw)
    minw = connect_b->sizeHint().width();

  minw = QMAX(minw,70);
  quit_b->setMinimumWidth(minw);
  setup_b->setMinimumWidth(minw);
  help_b->setMinimumWidth(minw);
  connect_b->setMinimumWidth(minw);

  l2->addWidget(quit_b);
  l2->addWidget(setup_b);
  l2->addWidget(help_b);
  l2->addSpacing(20);
  l2->addWidget(connect_b);

  tl->freeze();

  // we also connect cmld_start to the connectbutton so that I can run
  // the dialer through a command line argument
  connect(this,SIGNAL(cmdl_start()),this,SLOT(connectbutton())); 

  con_win = new ConWindow(0,"conw",this);
  KWM::setMiniIcon(con_win->winId(), *miniIcon);
  con_win->setGeometry(QApplication::desktop()->width()/2-160,
		    QApplication::desktop()->height()/2-55,
		    320,110);
  KWM::setMiniIcon(con_win->winId(), app->getMiniIcon());

  // connect to the accounting object
  connect(&accounting, SIGNAL(changed(QString, QString)),
	  con_win, SLOT(slotAccounting(QString, QString)));

  stats = new PPPStatsDlg(0,"stats",this);
  stats->hide();

  dock_widget = new DockWidget("dockw");

  debugwindow = new DebugWidget(0,"debugwindow");
  KWM::setMiniIcon(debugwindow->winId(), *miniIcon);
  debugwindow->setGeometry(QApplication::desktop()->width()/2+190,
		    QApplication::desktop()->height()/2-55,
		    debugwindow->width(),debugwindow->height());
  debugwindow->hide();

  // load up the accounts combo box

  resetaccounts();
  con = new ConnectWidget(0, "con");
  KWM::setMiniIcon(con->winId(), *miniIcon);
  connect(this, SIGNAL(begin_connect()),con, SLOT(preinit()));
  con->setGeometry(QApplication::desktop()->width()/2-175,
		    QApplication::desktop()->height()/2-55,
		    350,110);
    
  setGeometry(QApplication::desktop()->width()/2 - width()/2,
	      QApplication::desktop()->height()/2 - height()/2,
	      width(), height());

  if(have_cmdl_account){
    bool result;
    result = gpppdata.setAccount(cmdl_account.data());
    if (!result){
      QString string;
      string.sprintf(klocale->translate("No such Account:\n%s"),cmdl_account.data());
      QMessageBox::warning(this, klocale->translate("Error"), string.data());
      have_cmdl_account = false;
      this->show();
    }
    else {
      emit cmdl_start();
    }
  }
  else{
    this->show();
  }
}

void XPPPWidget::prepareSetupDialog() {
  if(tabWindow == 0) {
    tabWindow = new QTabDialog( 0, 0, TRUE );
    tabWindow->setCaption( klocale->translate("kppp Configuration") );
    tabWindow->setOkButton(klocale->translate("OK"));
    tabWindow->setCancelButton(klocale->translate("Cancel"));
    tabWindow->resize( 355, 350 );
    tabWindow->setFixedSize( 355, 350 ); // this doesn't seem to work in Qt 1.1
    
    accounts = new AccountWidget(tabWindow,"accounts");
    modem = new ModemWidget(tabWindow,"modem");
    modem2 = new ModemWidget2(tabWindow,"modem2");
    general = new GeneralWidget(tabWindow,"general");
    about  = new AboutWidget(tabWindow,"about");
    
    tabWindow->addTab( accounts, klocale->translate("Accounts") );
    tabWindow->addTab( modem, klocale->translate("Device") );
    tabWindow->addTab( modem2, klocale->translate("Modem") );
    tabWindow->addTab( general, klocale->translate("PPP") );
    tabWindow->addTab( about, klocale->translate("About") );
  }
}

void XPPPWidget::enterPressedInID() {
  PW_Edit->setFocus();
}

void XPPPWidget::enterPressedInPW() {
  connect_b->setFocus();
}

void XPPPWidget::log_window_toggled(bool on){
  
  gpppdata.set_show_log_window(on);
  
}


void XPPPWidget::setup()
{
  prepareSetupDialog();

  if(tabWindow->exec())
    gpppdata.save();
  else
    gpppdata.cancel();

}


void XPPPWidget::resetaccounts() {

  connectto_c->clear();

  if(gpppdata.count() == 0) {
    connectto_c->setEnabled(false);
    connect_b->setEnabled(false);
    log->setEnabled(false);
    ID_Edit->setEnabled(false);
    PW_Edit->setEnabled(false);
  }
  else {
    connectto_c->setEnabled(true);
    connect_b->setEnabled(true);
    log->setEnabled(true);
    ID_Edit->setEnabled(true);
    PW_Edit->setEnabled(true);
  }

  //load the accounts
  for(int i=0; i <= gpppdata.count()-1; i++) {
    gpppdata.setAccountbyIndex(i);
     connectto_c->insertItem(gpppdata.accname());
  }

  //set the default account
  if(strcmp(gpppdata.defaultAccount(), "") != 0) {
    for(int i=0; i < connectto_c->count(); i++)
       if(strcmp(gpppdata.defaultAccount(), connectto_c->text(i)) == 0) {
 	connectto_c->setCurrentItem(i);
	gpppdata.setAccountbyIndex(i);
	
	ID_Edit->setText(gpppdata.storedUsername());
	PW_Edit->setText(gpppdata.storedPassword());
    }
  }
  else 
    if(gpppdata.count() > 0) {
       gpppdata.setDefaultAccount(connectto_c->text(0));
    }


  connect(ID_Edit, SIGNAL(textChanged(const char *)),
 	  this, SLOT(usernameChanged(const char *)));

  connect(PW_Edit, SIGNAL(textChanged(const char *)),
 	  this, SLOT(passwordChanged(const char *)));
    
}


//Note: this is a friend function of XPPPWidget class (xppp)

void dieppp(int sig) {

  pid_t id;
  int st;

#ifdef MY_DEBUG
  printf("Received a signal: %d\n",sig);
#endif

  if(sig == SIGCHLD){

#ifdef MY_DEBUG
  printf("The Signal received was a SIGCHLD\n");
#endif

    signal(SIGCHLD, dieppp); // reinstall the sig handler
    id = wait(&st);

#ifdef MY_DEBUG
    printf("pid of the process which died: %d\n",id);
    printf("status of that process: %i\n", st);
#endif

    // if we are not connected pppdpid is -1 so have have to check for that
    // in the followin line to make sure that we don't raise a false alarm
    // such as would be the case when the log file viewer exits.

    if(id == gpppdata.pppdpid() && gpppdata.pppdpid() != -1) { 
      
#ifdef MY_DEBUG
  printf("It was pppd that died\n");
#endif
      // when we killppp() on Cancel in ConnectWidget 
      // we set pppid to -1 so we won't 
      // enter this block

      // just to be sure
      PAP_RemoveAuthFile();
      CHAP_RemoveAuthFile();

      gpppdata.setpppdpid(-1);

      if(strcmp(gpppdata.command_on_disconnect(), "") != 0) {
    
	pid_t id;

#ifdef MY_DEBUG
	printf(
	       "Executing command on disconnect since pppd has died: %s\n",
	       gpppdata.command_on_disconnect()
	       );
#endif

	if((id = fork()) == 0) {
	  setuid(getuid());
	  system(gpppdata.command_on_disconnect());
	  exit(0);
	}	 
      }


      p_xppp->stopAccounting();

      p_xppp->con_win->stopClock();
      dock_widget->stop_stats();
      dock_widget->undock();      

      pppd_has_died = true;
      removedns();
      unlockdevice();      
      
      if(!reconnect_on_disconnect) {
	p_xppp->quit_b->setFocus();
	p_xppp->show();
	p_xppp->con_win->stopClock();
	p_xppp->stopAccounting();
	p_xppp->con_win->hide();
	p_xppp->con->hide();

	gpppdata.setpppdpid(-1);
	
	app->beep();
	QString msg;
	switch (gpppdata.pppdError())
	  {
	  case E_IF_TIMEOUT:
	    msg = "Timeout expired on waiting for the ppp interface ";
	    msg += "to come up!";
	    break;
	  
	  default: 
	    msg = "The pppd daemon died unexpectedly!";
	  }
	
	QMessageBox::warning(0, klocale->translate("Error"), 
			     klocale->translate(msg));
  }
      else{/* reconnect on disconnect */
#ifdef MY_DEBUG
  printf("Trying to reconnect ... \n");
#endif
        if(PAP_UsePAP())
	  PAP_CreateAuthFile();

        if(CHAP_UseCHAP())
	  CHAP_CreateAuthFile();

	p_xppp->con_win->hide();
	p_xppp->con_win->stopClock();
	p_xppp->stopAccounting();
	gpppdata.setpppdpid(-1);
	app->beep();
	emit p_xppp->cmdl_start();
      }
    }
  }
}



void XPPPWidget::newdefaultaccount(int i) {
  gpppdata.setDefaultAccount(connectto_c->text(i));
  gpppdata.save();
  ID_Edit->setText(gpppdata.storedUsername());
  PW_Edit->setText(gpppdata.storedPassword());
}


void XPPPWidget::expandbutton() {
  setup();
}


void XPPPWidget::connectbutton() {

  // make sure to connect to the account that is selected in the combo box
  // (exeption: an account given by a command line argument)
  if(!have_cmdl_account) gpppdata.setAccount(connectto_c->currentText());

  QFileInfo info(gpppdata.pppdPath());

  if(!info.exists()){
    QString string;   
    string.sprintf(klocale->translate("kppp can not find:\n %s\nPlease install pppd properly "
				      "and/or adjust\n the location of the pppd executable on "
				      "the PPP tab of\n"
				      "the setup dialog.\n Thank You"),
		   gpppdata.pppdPath());
    QMessageBox::warning(this, klocale->translate("Error"), string.data());
    return;
  }

  if(!info.isExecutable()){

    QString string;   
    string.sprintf(klocale->translate("kppp can not execute:\n %s\nPlease make sure that"
		   "you have given kppp setuid permission and that\n"
		   "pppd is executable."),gpppdata.pppdPath());
    QMessageBox::warning(this, 
			 klocale->translate("Error"),
			 string.data());
    return;

  }

  QFileInfo info2(gpppdata.modemDevice());

  if(!info2.exists()){
    QString string;   
    string.sprintf(klocale->translate("kppp can not find:\n %s\nPlease make sure you have setup\n"
		   "your modem device properly\n"
		   "and/or adjust\n the location of the modem device on "
		   "the modem tab of\n"
		   "the setup dialog.\n Thank You"),gpppdata.modemDevice());
    QMessageBox::warning(this, 
			 klocale->translate("Error"),
			 string.data());
    return;
  }

  gpppdata.setPassword(PW_Edit->text());

  // if this is a PAP account, ensure that password and username are
  // supplied
  if(PAP_UsePAP()) {
    if(strlen(ID_Edit->text()) == 0 || strlen(PW_Edit->text()) == 0) {
      QMessageBox::warning(this,
			   klocale->translate("Error"),
			   klocale->translate(
                           "You have selected the authentication\n"
			   "method PAP. This requires that you\n"
			   "supply a username and a password!"));
      return;
    } else {      
      if(!PAP_CreateAuthFile()) {
	QString s;
	s.sprintf(klocale->translate("Cannot create PAP authentication\n"
				     "file \"%s\""), PAP_AUTH_FILE);
	QMessageBox::warning(this,
			     klocale->translate("Error"),
			     s.data());
	return;
      }
    }
  }

  // if this is a CHAP account, ensure that password and username are
  // supplied
  if(CHAP_UseCHAP()) {
    if(strlen(ID_Edit->text()) == 0 || strlen(PW_Edit->text()) == 0) {
      QMessageBox::warning(this,
			   klocale->translate("Error"),
			   klocale->translate(
                           "You have selected the authentication\n"
			   "method CHAP. This requires that you\n"
			   "supply a username and a password!"));
      return;
    } else {      
      if(!CHAP_CreateAuthFile()) {
	QString s;
	s.sprintf(klocale->translate("Cannot create CHAP authentication\n"
				     "file \"%s\""), CHAP_AUTH_FILE);
	QMessageBox::warning(this,
			     klocale->translate("Error"),
			     s.data());
	return;
      }
    }
  }
  
  this->hide();

  QString tit = klocale->translate("Connecting to: ");
  tit += gpppdata.accname();
  con->setCaption(tit);

  con->show();

  if (!(log->isChecked())){
    debugwindow->hide();
  }
  else{
    debugwindow->clear();
    debugwindow->show();
    con->raise();
    con->debug->setText(klocale->translate("Log")); // set Log/Hide button text to Hide
  }	
  
  debugwindow->clear();
  emit begin_connect();
}


void XPPPWidget::disconnect() {

  app->processEvents();

  stats->stop_stats();
  terminatepppd();
  gpppdata.setpppdpid(-1);
  
  PAP_RemoveAuthFile();
  CHAP_RemoveAuthFile();

  removedns();
  unlockdevice();
  
  con_win->stopClock();
  p_xppp->stopAccounting();
  con_win->hide();
  
  if (dock_widget->isDocked()) {
    dock_widget->stop_stats();
    dock_widget->undock();
  }

  if(quit_on_disconnect) {
    kapp->exit(0);
  } else {
    p_xppp->quit_b->setFocus();
    p_xppp->show();
  }
}



void XPPPWidget::helpbutton() {

  app->invokeHTMLHelp("kppp/kppp.html","");

}             



void XPPPWidget::quitbutton() {

  if(gpppdata.pppdpid() >= 0) {

    bool ok = QMessageBox::query(klocale->translate("Quit kPPP?"), 
				 klocale->translate("Exiting kPPP will close your PPP Session."),
				 klocale->translate("Yes"),
				 klocale->translate("No"));

    if(ok) {
      terminatepppd();
      gpppdata.setpppdpid(-1);
      removedns();
      unlockdevice();
      qApp->quit();
    }
  }
  else {
    if (strcmp(gpppdata.accname(), "") != 0 && !gpppdata.storePassword())
      gpppdata.setStoredPassword("");
    kapp->quit();
  }

}

void XPPPWidget::rulesetLoadError() {
  QMessageBox::warning(this, 
		       klocale->translate("Error"), 
		       ruleset_load_errmsg.data());
}

void XPPPWidget::startAccounting() {
  // volume accounting
  totalbytes = 0;

  // load the ruleset
  if(!gpppdata.AcctEnabled())
    return;
  
  if(!accounting.loadRuleSet(gpppdata.accountingFile())) {
    QString s= klocale->translate("Can not load the accounting\nruleset \"");
    s += gpppdata.accountingFile();
    s += "\"!";

    // starting the messagebox with a timer will prevent us
    // from blocking the calling function ConnectWidget::timerEvent
    ruleset_load_errmsg = s;
    QTimer::singleShot(0, this, SLOT(rulesetLoadError()));
    return;
    } 
  else
    {
	accounting.slotStart();
    }
}

void XPPPWidget::stopAccounting() {

  // store volume accounting
  if(totalbytes != 0)
    gpppdata.setTotalBytes(totalbytes);

  if(!gpppdata.AcctEnabled())
    return;

  accounting.slotStop();
}

void XPPPWidget::usernameChanged(const char *) {

  // store username for later use
  gpppdata.setStoredUsername(ID_Edit->text());
}

void XPPPWidget::passwordChanged(const char *) {
  
  // store the password if so requested
  if(gpppdata.storePassword())
    gpppdata.setStoredPassword(PW_Edit->text());
  else
    gpppdata.setStoredPassword("");
}

void XPPPWidget::setPW_Edit(const char *pw) {

  PW_Edit->setText(pw);

}


void terminatepppd() {
  
#ifdef MY_DEBUG
printf("In terminatepppd(): I will attempt to kill pppd\n");
#endif

  int stat;

  if(gpppdata.pppdpid() >= 0) {


    if(kill(gpppdata.pppdpid(), SIGTERM) < 0)
      qApp->beep();
      
    wait(&stat);

    gpppdata.setpppdpid(-1);

      if(strcmp(gpppdata.command_on_disconnect(), "") !=0) {
    
	pid_t id;

#ifdef MY_DEBUG
  printf(
	 "Executing command on disconnect in terminatedpppd(): %s\n",
	 gpppdata.command_on_disconnect()
	 );
#endif

	if((id = fork()) == 0) {
	  setuid(getuid());
	  system(gpppdata.command_on_disconnect());
	  exit(0);
	}	 
      }
    
  }

  gpppdata.setpppdpid(-1);

}

#include "main.moc"
