/**********************************************************************

 TopLevel IRC Channel/query Window

 $$Id$$

 This is the main window with with the user interacts.  It handles
 both normal channel converstations and private conversations.

 2 classes are defined, the UserControlMenu and KSircToplevel.  The
 user control menu is used as alist of user defineable menus used by
 KSircToplevel.  

 KSircTopLevel:

 Signals: 

 outputLine(QString &): 
 output_toplevel(QString):

 closing(KSircTopLevel *, char *): 

 changeChannel(QString old, QString new): 

 currentWindow(KSircTopLevel *):

 changeSize(): 

 Slots: 
 

 
 *********************************************************************/

#include "toplevel.h"
#include "iocontroller.h"
#include "open_top.h"
#include "control_message.h"
#include "config.h"
#include "KSCutDialog/KSCutDialog.h"
#include "ssfeprompt.h"

#include <iostream.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include <qclipbrd.h> 
#include <qmsgbox.h> 
#include <qtooltip.h>

#include <knewpanner.h>
#include <kiconloader.h>

extern KConfig *kConfig;
extern KApplication *kApp;

//QPopupMenu *KSircTopLevel::user_controls = 0L;
QList<UserControlMenu> *KSircTopLevel::user_menu = 0L;
QPixmap *KSircTopLevel::pix_info = 0L;
QPixmap *KSircTopLevel::pix_star = 0L;
QPixmap *KSircTopLevel::pix_bball = 0L;
QPixmap *KSircTopLevel::pix_greenp = 0L;
QPixmap *KSircTopLevel::pix_bluep = 0L;
QPixmap *KSircTopLevel::pix_madsmile = 0L;

KSircTopLevel::KSircTopLevel(KSircProcess *_proc, char *cname=0L, const char * name=0) 
  : KTopLevelWidget(name),
    KSircMessageReceiver(_proc)
   
{

  /*
   * Setup window for operation.  We don't handle io!
   */

  /*
   * QPopup title bar, does not actually do anything at this time
   */

  proc = _proc;

  channel_name = qstrdup(cname);
  if(channel_name){
  //    QString s = channel_name;
  //    int pos2 = s.find(' ', 0);
  //    if(pos2 > 0)
  //      channel_name = qstrdup(s.mid(0, pos2).data());
    
    setCaption(channel_name);
    caption = channel_name;
  }
  else
    caption = "";

  LineBuffer = new QStrList();
  Buffer = FALSE;

  have_focus = 0;
  ticker = 0; // Set the ticker to NULL while it doesn't exist.
  tab_pressed = 0; // Tab (nick completion not pressed yet)


  /*
   * Create the status bar which will hold the ping time and status info
   */
  ktool = new KToolBar(this, "toolbar");
  ktool->setFullWidth(TRUE);
  ktool->insertFrame(0, 200, 0);
  ktool->setItemAutoSized(0, TRUE);
  ktool->insertFrame(10, 100, -1);
  ktool->alignItemRight(10, TRUE);
  addToolBar(ktool);

  lagmeter = new QLCDNumber(6, ktool->getFrame(10), "lagmeter");
  lagmeter->setFrameStyle(QFrame::NoFrame);
  lagmeter->setFixedHeight(ktool->height() - 2);
  lagmeter->display("      ");
  QToolTip::add(lagmeter, "Lag in seconds to the server");
  
  QPopupMenu *file = new QPopupMenu();
  file->insertItem("&New Window...", this, SLOT(newWindow()), CTRL + Key_N);
  file->insertItem("&Ticker Mode", this, SLOT(showTicker()), CTRL + Key_T);
  file->insertSeparator();
  file->insertItem("&Quit", this, SLOT(terminate()), CTRL + Key_Q );

  QFrame *menu_frame = ktool->getFrame(0);
  CHECK_PTR(menu_frame);
  menu_frame->setFrameStyle(QFrame::NoFrame); // Turn off the frame style.
  menu_frame->setLineWidth(0);
  
  kmenu = new QMenuBar(this, "menubar");
  //  kmenu = new QMenuBar(menu_frame, "menubar");
  //  kmenu = new QMenuBar(this, "menubar");
  kmenu->setFrameStyle(QFrame::NoFrame); // Turn off frame style.
  kmenu->setLineWidth(0);  
  if(style() == MotifStyle)
    kmenu->recreate(menu_frame, 0, QPoint(0,-3));
  else
    kmenu->recreate(menu_frame, 0, QPoint(0,0));
  kmenu->insertItem("&File", file, 2, -1);
  kmenu->setAccel(Key_F, 2);
  //  topLevelWidget()->installEventFilter(kmenu);

  QPopupMenu *edit = new QPopupMenu();
  edit->insertItem("&Cut WIndow...", this, SLOT(openCutWindow()), CTRL + Key_X);
  edit->insertItem("&Paste", this, SLOT(pasteToWindow()), CTRL + Key_V);
  kmenu->insertItem("&Edit", edit, -1, -1);

  //  setMenu(kmenu);

  /*
   * Ok, let's look at the basic widge t "layout"
   * Everything belongs to q QFrame F, this is use so we
   * can give the KApplication a single main client widget, which is needs.
   *
   * A QVbox and a QHbox is used to ctronl the 3 sub widget
   * The Modified QListBox is then added side by side with the User list box.
   * The SLE is then fit bello.
   */

  QFrame *f = new QFrame(this, "frame");
  setView(f);  // Tell the KApplication what the main widget is.

  gm = new QVBoxLayout(f, 5); // Main top layout
  gm2 = new QHBoxLayout(10);   // Layout for users text and users box
  gm->addLayout(gm2, 10);

  //  mainw = new QListBox(f, "mle");          // Make a flat QListBox.  I want the

  if(kSircConfig->colour_background == 0){
    kConfig->setGroup("Colours");
    kSircConfig->colour_background = new QColor(kConfig->readColorEntry("Background", new QColor(colorGroup().mid())));
  }

  pan = new KNewPanner(f, "knewpanner", KNewPanner::Vertical,
		       KNewPanner::Absolute, width()-1000);
  gm2->addWidget(pan, 10);

  mainw = new KSircListBox(pan, "mle");
  mainw->setFocusPolicy(QWidget::NoFocus); // Background and base colour of
  //mainw->setFocusPolicy(QWidget::StrongFocus); // Background and base colour of
  mainw->setEnabled(FALSE);                // the lb to be the same as the main
  mainw->setSmoothScrolling(TRUE);         // ColourGroup, but this is BAD BAD
  mainw->setFont(kSircConfig->defaultfont);// Since we don't use KDE requested

  connect(mainw, SIGNAL(updateSize()),
	  this, SIGNAL(changeSize()));
  QColorGroup cg = QColorGroup(*kSircConfig->colour_text, colorGroup().mid(), 
    			       colorGroup().light(), colorGroup().dark(),
  			       colorGroup().midlight(), 
  			       *kSircConfig->colour_text, 
			       *kSircConfig->colour_background); 
  mainw->setPalette(QPalette(cg,cg,cg));   // colours.  Font it also hard coded
  mainw->setMinimumWidth(width() - 100);
  //  gm2->addWidget(mainw, 10);               // which is bad bad.

  nicks = new aListBox(pan, "qlb");          // Make the users list box.
  //nicks->setMaximumWidth(100);             // Would be nice if it was flat and
  //  nicks->setMinimumWidth(100);             // matched the main text window
  nicks->setFocusPolicy(QWidget::NoFocus);
  nicks->setPalette(QPalette(cg,cg,cg));   // HARD CODED COLOURS AGAIN!!!!
  nicks->setFont(kSircConfig->defaultfont);
  //  gm2->addWidget(nicks, 0);

  pan->activate(mainw, nicks);
  pan->setAbsSeparatorPos(width() - 100);
  //  mainw->setMinimumWidth(0);             // matched the main text window

  linee = new aHistLineEdit(f, "qle");        // aHistEdit is a QLineEdit with 
                                              // arrows for 
  linee->setFocusPolicy(QWidget::StrongFocus); // scroll back abillity
  linee->setPalette(QPalette(cg,cg,cg));   // HARD CODED COLOURS AGAIN!!!! (last time I hope!)
  linee->setFont(kSircConfig->defaultfont);
  linee->resize(width(), linee->fontMetrics().height() + 8);
  linee->setMinimumHeight(linee->fontMetrics().lineSpacing()+8);
  connect(linee, SIGNAL(gotFocus()),
	  this, SLOT(gotFocus()));
  connect(linee, SIGNAL(lostFocus()),
	  this, SLOT(lostFocus()));
  connect(linee, SIGNAL(pasteText()),
	  this, SLOT(pasteToWindow()));
  connect(linee, SIGNAL(textChanged(const char *)),
	  this, SLOT(lineeTextChanged(const char *)));

  gm->addWidget(linee, 0);                    // No special controls are needed.

  connect(linee, SIGNAL(returnPressed()), // Connect return in sle to send
  	  this, SLOT(sirc_line_return()));// the line to dsirc
 
  linee->setFocus();  // Give SLE focus

  lines = 0;          // Set internal line counter to 0
  contents.setAutoDelete( TRUE ); // Have contents, the line holder nuke everything on exit

  /*
   * Set generic run time variables
   *
   */

  opami = FALSE;
  continued_line = FALSE;
  prompt_active = FALSE;

  /*
   * Load basic pics and images
   * This should use on of the KDE finder commands
   * CHANGE THIS AWAY FROM HARD CODED PATHS!!!
   */

  if(pix_info == 0){
    KIconLoader *kicl = kApp->getIconLoader();
    QStrList *strlist = kicl->getDirList();
    kicl->insertDirectory(strlist->count(), kSircConfig->kdedir + "/share/apps/ksirc/icons");
    pix_info = new QPixmap(kicl->loadIcon("info.gif"));
    pix_star = new QPixmap(kicl->loadIcon("star.gif"));
    pix_bball = new QPixmap(kicl->loadIcon("blueball.gif"));
    pix_greenp = new QPixmap(kicl->loadIcon("greenpin.gif"));
    pix_bluep = new QPixmap(kicl->loadIcon("bluepin.gif"));
    pix_madsmile = new QPixmap(kicl->loadIcon("madsmiley.gif"));
  }

  /* 
   * Create the user Controls popup menu, and connect it with the
   * nicks list on the lefthand side (highlighted()).
   * 
   */

  //  if(!user_controls)
  //    user_controls = new QPopupMenu();

  if(user_menu == 0)
    user_menu = UserControlMenu::parseKConfig();

  user_controls = new QPopupMenu();
  kmenu->insertItem("&Users", user_controls);

  connect(user_controls, SIGNAL(activated(int)), 
	  this, SLOT(UserParseMenu(int)));

  connect(nicks, SIGNAL(rightButtonPress(int)), this,
	  SLOT(UserSelected(int)));


  UserUpdateMenu();  // Must call to update Popup.

  accel = new QAccel(this, "accel");

  accel->connectItem(accel->insertItem(SHIFT + Key_PageUp),
		     this,
		     SLOT(AccelScrollUpPage()));
  accel->connectItem(accel->insertItem(SHIFT + Key_PageDown),
		     this,
		     SLOT(AccelScrollDownPage()));

  accel->connectItem(accel->insertItem(CTRL + Key_Enter),
		     this,
		     SLOT(AccelPriorMsgNick()));

  accel->connectItem(accel->insertItem(CTRL + SHIFT + Key_Enter),
		     this,
		     SLOT(AccelNextMsgNick()));

  accel->connectItem(accel->insertItem(CTRL + Key_Return),
		     this,
		     SLOT(AccelPriorMsgNick()));

  accel->connectItem(accel->insertItem(CTRL + SHIFT + Key_Return),
		     this,
		     SLOT(AccelNextMsgNick()));

  accel->connectItem(accel->insertItem(Key_Tab), // adds TAB accelerator
                     this,                         // connected to the main
                     SLOT(TabNickCompletion()));  // TabNickCompletion() slot
  accel->connectItem(accel->insertItem(CTRL + Key_N),
		     this, SLOT(newWindow()));
  accel->connectItem(accel->insertItem(CTRL + Key_T),
		     this, SLOT(showTicker()));
  accel->connectItem(accel->insertItem(CTRL + Key_Q),
		     this, SLOT(terminate()));
  accel->connectItem(accel->insertItem(ALT + Key_F4),
		     this, SLOT(terminate()));


}


KSircTopLevel::~KSircTopLevel()
{

  // Cleanup and shutdown
  //  writePopUpMenu();
  //  if(this == proc->getWindowList()["default"])
  //    write(sirc_stdin, "/quit\n", 7); // tell dsirc to close
  //
  //  if(proc->getWindowList()[channel_name])
  //    proc->getWindowList().remove(channel_name);

  //  if((channel_name[0] == '#') || (channel_name[0] == '&')){
  //    QString str = QString("/part ") + channel_name + "\n";
  //    emit outputLine(str);
  //  }

  if((channel_name[0] == '#') || (channel_name[0] == '&')){
    QString str = QString("/part ") + channel_name + "\n";
    emit outputLine(str);
  }

  if(ticker)
    delete ticker;
  ticker = 0;
  //  delete gm; // Deletes everthing bellow it I guess...
  //  delete gm2; 
  //  delete pan; // Should be deleted by gm2
  //  delete linee; // ditto
  delete LineBuffer;
  delete user_controls;

  QToolTip::remove(lagmeter);
  delete kmenu;
  delete ktool;

  //  close(sirc_stdin);  // close all the pipes
  //  close(sirc_stdout); // ditto
  //  close(sirc_stderr); // duh... ;)

}

void KSircTopLevel::show()
{
  if(ticker){
    ticker->show();
    ticker->raise();
  }
  else{
    KTopLevelWidget::show();
    kmenu->show();
    lagmeter->show();
  }
}

//void KSircTopLevel::sirc_stop(bool STOP = FALSE)
//{
//  if(STOP == TRUE){
//    Buffer = TRUE;
//  }
//  else{
//    Buffer = FALSE;
//    if(LineBuffer->isEmpty() == FALSE)
//      sirc_receive(QString(""));
//  }
//}

void KSircTopLevel::TabNickCompletion() 
{
  /* 
   * Gets current text from lined find the last item and try and perform
   * a nick completion, then return and reset the line.
   */

  int pos;
  QString s;

  if(tab_pressed > 0)
    s = tab_saved.data();
  else{
    s = linee->text();
    tab_saved = s.data();
  }

  if(s.length() == 0)
    return;

  pos = s.findRev(" ", -1, FALSE);

  if (pos == -1) {
    QString nick = findNick(s, tab_pressed);
    if(nick.isNull() == TRUE){
      tab_pressed = 0;
      nick = findNick(s, tab_pressed);
    }
    s = nick;
  }
  else {
    QString nick = findNick(s.mid(pos + 1, s.length()), tab_pressed);
    if(nick.isNull() == TRUE){
      tab_pressed = 0;
      nick = findNick(s.mid(pos + 1, s.length()), tab_pressed);
    }
    s.replace(pos + 1, s.length(), nick);
  }

  tab_pressed++;

  disconnect(linee, SIGNAL(textChanged(const char *)),
	  this, SLOT(lineeTextChanged(const char *)));

  linee->setText(s);

  connect(linee, SIGNAL(textChanged(const char *)),
	  this, SLOT(lineeTextChanged(const char *)));

}
  
void KSircTopLevel::sirc_receive(QString str)
{

  /* 
   * read and parse output from dsirc.
   * call reader, split the read input into lines, prase the lines
   * then print line by line into the main text area.
   *
   * PROBLEMS: if a line terminates in mid line, it will get borken oddly
   *
   */

  /*
   * If we have to many lines, nuke the top 100, leave us with 100
   */

  int lines = 0;

  if(Buffer == FALSE){
    if(LineBuffer->count() >= 2){
      mainw->setAutoUpdate(FALSE);
    }
    
    if(str.isEmpty() == FALSE){
      LineBuffer->append(str);
    }

    ircListItem *item = 0;
    char *pchar;
    QString string;
    bool update = FALSE;

    for(pchar = LineBuffer->first(); 
	pchar != 0; 
	LineBuffer->removeFirst(),     // Remove the first one
	  pchar=LineBuffer->first()){  // Get the new first one
      // Get the need list box item, with colour, etc all set
      string = pchar;
      item = parse_input(string);
      // If we shuold add anything, add it.
      // Item might be null, if we shuoold ingore the line
      
      if(item){
	// Insert line to the end
	connect(this, SIGNAL(changeSize()),
		item, SLOT(updateSize()));
	mainw->insertItem(item, -1);
	if(ticker){
	  QString text;
	  int colour = KSPainter::colour2num(*(item->defcolour()));
	  if(colour >= 0){
	    text.setNum(colour);
	    text.prepend("~");
	  }
	  text.append(item->getText());
	  ticker->mergeString(text + "~C // ");
	}
	lines++; // Mode up lin counter
	update = TRUE;
      }
//      if(mainw->count() > 100)
//	mainw->removeItem(0);
    }
    LineBuffer->clear(); // Clear it since it's been added

    if(mainw->count() > 200){
        mainw->setAutoUpdate(FALSE);
        update = TRUE;
        while(mainw->count() > 100)
           mainw->removeItem(0);
    }

    //    if(mainw->autoUpdate() == FALSE){
    mainw->setAutoUpdate(TRUE);
      //      mainw->update();
      //      mainw->repaint(TRUE);
      //    }

    // If we need to scroll, we, scroll =)
    // scrollToBottom returns true if we should repaint.
    mainw->scrollToBottom();
  }
  else{
    LineBuffer->append(str);
  }
}

void KSircTopLevel::sirc_line_return()
{

  /* Take line from SLE, and output if to dsirc */

  QString s = linee->text();

  if(s.length() == 0)
    return;

  tab_pressed = 0; // New line, zero the counter.

  // 
  // Lookup the nick completion
  // Do this before we append the linefeed!!
  //

  int pos2;
  
  if(kSircConfig->nickcompletion == TRUE){
    if(s.find(QRegExp("^[^ :]+: "), 0) != -1){
      pos2 = s.find(": ", 0);
      if(pos2 < 1){
	cerr << "Evil string: " << s << endl;
      }
      else
	s.replace(0, pos2, findNick(s.mid(0, pos2)));
    }
    
    //    pos2 = 0;
    //    pos1 = 0;
    //    
    //    while(s.find(" ::", pos2) >= 0){
    //      pos1 = s.find(" ::", pos2);
    //      pos2 = s.find(" ", pos1+3);
    //      if(pos2 == -1)
    //	pos2 = s.length();
    //      if(pos2 - pos1 - 3 < 1){
    //	cerr << "Evil string: " << s << endl;
    //	break;
    //      }
    //      else{
    //	s.replace(pos1 + 1, pos2 - pos1 - 1, 
    //		  findNick(s.mid(pos1 + 3, pos2 - pos1 - 3)));
    //      }
    //    }
  }

  s += '\n'; // Append a need carriage return :)

  /*
   * Parse line forcommand we handle
   */

  if((strncmp(s, "/join ", 6) == 0) || 
     (strncmp(s, "/j ", 3) == 0) ||
     (strncmp(s, "/query ", 7) == 0)){
    s = s.lower();
    int pos1 = s.find(' ', 0) + 1;
    if(pos1 == -1)
      return;
    while(s[pos1] == ' ')
      pos1++;
    int pos2 = s.length() - 1;
    if(pos1 > 2){
      QString name = s.mid(pos1, pos2 - pos1); // make sure to remove line feed
      emit open_toplevel(name);
      if(name[0] != '#'){
	linee->setText("");
	return;
      }
    }
  }
  else if(strncmp(s, "/server ", 6) == 0){
    QString command = "/eval &print(\"*E* Use The Server Controller\\n\");\n";
    sirc_write(command);
    linee->setText("");
    return;
  }
  else if((strncmp(s, "/part", 5) == 0) ||
	  (strncmp(s, "/leave", 6) == 0) ||
	  (strncmp(s, "/hop", 4) == 0) ||
	  (strncmp(s, "/quit", 5) == 0)){
    QApplication::postEvent(this, new QCloseEvent()); // WE'RE DEAD
    linee->setText("");
    s.truncate(0);
    return;
  }

  // 
  // Look at the command, if we're assigned a channel name, default
  // messages, etc to the right place.  This include /me, etc
  //

  if((uint) nick_ring.at() < (nick_ring.count() - 1))
    nick_ring.next();
  else
    nick_ring.last();

  sirc_write(s);

  linee->setText("");
  
}

void KSircTopLevel::sirc_write(QString &str)
{
  if(channel_name[0] != '!'){
    if(str[0] != '/'){
      str.prepend(QString("/msg ") + channel_name + QString(" "));
    }
    else if(strnicmp(str, "/me", 3) == 0){
      str.remove(0, 3);
      str.prepend(QString("/de ") + channel_name);
    }
  }
  
  // Write out line

  //  proc->stdin_write(str);
  mainw->scrollToBottom(TRUE);
  emit outputLine(str);

}

ircListItem *KSircTopLevel::parse_input(QString &string)
{

  /* 
   * welcome to the twilight zone
   * Big time parsing, and no docs
   * Variables are reused unsafly, big EWWW!
   */

  /* 
   * Parsing routines are broken into 3 majour sections 
   *
   * 1. Search and replace evil characters. The string is searched for
   * each character in turn (evil[i]), and the character string in
   * evil_rep[i] is replaced.  
   *
   * 2. Parse control messages, add pixmaps, and colourize required
   * lines.  Caption is also set as required.
   *
   * 3. Create and return the ircListItem.
   *
   */

  /*
   * c: main switch char, used to id *'s and ssfe commands
   * s2: temporary string pointer, used is parsing title string
   * evil[]: list of eveil characters that should be removed from 
   *         the data stream.
   * evil_rep[][], list of characters to fill replace evil[] with
   * s3: temp Qstring
   * s4: temp QString
   * pos: start position indicator, used for updating nick info
   * pos2: end position indicator, tail end for nick info
   * color: colour for ListBox line entry
   * pixmap: pixmap for left hand side of list box
   * */

  char c,*s2;

  // \n: clear any line feeds                              -> " "
  // \r: clear stray carriage returns                      -> ""
  // \002: control character of some kinda, used for bold? -> ""
  // \037: bold or underline, evil messy char              -> Not Used
  // \000: terminating null
  char evil[] = {'\n', '\r', '\002', '\037', '\000'};
  char *evil_rep[] = {
    " ",
    "",
    "",
    ""
  };
  QString s3, s4, channel;
  int pos, pos2;
  QColor *color = kSircConfig->colour_text;
  QPixmap *pixmap = NULL;

  /*
   * No-output get's set to 1 if we should ignore the line
   */

  int no_output = 0;

  for(int i = 0; evil[i] != 0; i++){
    pos = string.find(evil[i], 0, FALSE);  // look for first occurance
    while(pos >= 0){                       // If found, start stepping
      string.remove(pos, 1);               // Remove evil char
      string.insert(pos, evil_rep[i]);     // insert replacement
      pos = string.find(evil[i], pos+strlen(evil_rep[i]), FALSE);
					   // find next
    }
  }

  //  if(string[0] == '~'){
  //    pos = 1;
  //    pos2 = string.find("~", pos);
  //    if(pos2 > pos){
  //      channel = string.mid(pos, pos2-pos); // s3 now holds the channel name
  //      channel = channel.lower();           // lower case s3
  //      string.remove(pos-1, pos2-pos+2);
  //    }
  //  }
  //
  //  if(channel.isEmpty() == FALSE)
  //    cerr << "Channel Specefic: " << channel << endl;

  //  strncpy(&c, string.left(1), 1);          // Copy the first char into
					   // c, and then do a switch
					   // on it

  switch(string[0]){
  case '`':                                // ` is an ssfe command
    s2 = strstr(string, "#ssfe#");
    if(s2 > 0){
      s2+=6;                               // move ahead character end
					   // of `ssfe control
					   // message, switch on end
					   // of control char
      //      cerr << "s2: " << s2;
      switch(s2[0]){
      case 's':                            // moved [sirc] message
	s2+=10;                            // set the rest of the line
					   // to the caption
	if(s2 != caption){
	  if(s2[0] == '@')                 // If we're an op,, 
	                                   // update the nicks popup menu
	    opami = TRUE;                  // opami = true sets us to an op
	  else
	    opami = FALSE;                 // FALSE, were not an ops
	  UserUpdateMenu();                // update the menu
	  setCaption(s2);
	  if(ticker)
	    ticker->setCaption(s2);
	  caption = s2;           // Make copy so we're not 
	                                   // constantly changing the title bar
	}
	no_output = 1;                     // Don't print caption
	break;
      case 'i':
	string.truncate(0);                // truncate string... set
					   // no output, what's i?
	no_output = 1;
	break;
      case 't':
	no_output = 1;
	pos = string.find("t/m ", 6);
	if(pos >= 0){
	  pos += 4;
	  pos2 = string.find(" ", pos);
	  if(pos2 == -1)
	    pos2 = string.length();
	  if(pos2 > pos){
	    if(!nick_ring.contains(string.mid(pos, pos2-pos))){
	      nick_ring.append(string.mid(pos, pos2-pos));
	      //cerr << "Appending: " << string.mid(pos, pos2-pos) << endl;
	      if(nick_ring.count() > 10)
		nick_ring.removeFirst();
	    }
	  }
	  break;
	}
      case 'o':
	no_output = 1;
	string.truncate(0);
	break;
      case 'l':
	mainw->clear();
	mainw->repaint(TRUE);
	string.truncate(0);
	no_output = 1;
	break;
      case 'P':
      case 'p':
	{
	  if(prompt_active == FALSE){
	    QString prompt, caption;
	    ssfePrompt *sp;
	    int p1, p2;

	    // Flush the screen.
	    // First remove the prompt message from the Buffer.
	    // (it's garunteed to be the first one)
	    LineBuffer->removeFirst();
	    Buffer = FALSE;
	    sirc_receive(QString(""));
	    
	    caption = mainw->text(mainw->count() - 1);
	    if(caption.length() < 3){
	      caption = mainw->text(mainw->count() - 2);
	      if(caption.length() > 2)
		mainw->removeItem(mainw->count() - 2 );
	    }
	    else
	      mainw->removeItem(mainw->count() - 1 );
	    p1 = string.find("ssfe#", 0) + 6; // ssfe#[pP] == 6
	    p2 = string.length();
	    if(p2 <= p1)
	      prompt = "No Prompt Given?";
	    else
	      prompt = string.mid(p1, p2 - p1);
	    prompt_active = TRUE;
	    // If we use this, then it blows up
	    // if we haven't popped up on the remote display yet.
	    sp = new ssfePrompt(prompt, 0); 
	    sp->setCaption(caption);
	    if(s2[0] == 'P')
	      sp->setPassword(TRUE);
	    sp->exec();
	    //	  cerr << "Entered: " << sp->text() << endl;
	    prompt = sp->text();
	    prompt += "\n";
	    emit outputLine(prompt);
	    delete sp;
	    prompt_active = FALSE;
	    string.truncate(0);
	    no_output = 1;
	    break;
	  }
	}
	cerr << "Prompt already open!!!\n";
	break;
      case 'R': // Reconnect, join channels, etc if needed.
	if(channel_name[0] == '#'){
	  QString str = "/join " + QString(channel_name) + "\n";
	  emit outputLine(str);
	}
	string.truncate(0);                // truncate string... set
	no_output = 1;
	break;
      default:
	cerr << "Unkown ssfe command: " << string << endl;
	string.truncate(0);                // truncate string... set
	no_output = 1;
      }
    }
    break;                                 // stop ssfe controls...
  case '*': // Parse and control ssfe control messages...
    if(string.length() > 2){ // Chack string, make sure it's valid
      strncpy(&c, string.mid(2,1), 1); // We double check that the value isn't the nasty control char that seems to slip through the evil char check
      if(c == '\002'){
	string.remove(2,1);
	strncpy(&c, string.mid(2,1), 1);
      }
      string.remove(0, 1);

      if(string[0] != '#')                  // It's not a users line, so we're not on continuing
	continued_line = FALSE;             // a long user list

      c = string[0];
      switch(c){
      case '*':                             // * is an info message
	string.remove(0, 2);                // takes off the junk
	if(string.contains("Talking to")){
	  cerr << "Removing Talking to\n";
	  string.truncate(0);
	  no_output = 1;
        }
	pixmap = pix_info;                 // Use the I/blue cir pixmap
	color = kSircConfig->colour_info;   // Colour is blue for info
	break;
      case 'E':                            // E's an error message
	string.remove(0, 2);               // strip the junk
	pixmap = pix_madsmile;             // use the mad smiley
	color = kSircConfig->colour_error;  // set the clour to red
	break;
      case '#':                             // Channel listing of who's in it
	nicks->setAutoUpdate(FALSE);        // clear and update nicks
	if(continued_line == FALSE)
	  nicks->clear();
	continued_line = TRUE;
	pos = string.find(": ", 0, FALSE) + 1; // Find start of nicks
	while(pos > 0){                     // While there's nick to go...
	  pos2 = string.find(" ", pos + 1, FALSE); // Find end of nick
	  if(pos2 < pos)
	    pos2 = string.length();         // If the end's not found, 
	                                    // set to end of the string
	  s3 = string.mid(pos+1, pos2 - pos - 1); // Get nick
	  if(s3[0] == '@'){    // Remove the op part if set
	    s3.remove(0, 1);
	    ircListItem *irc = new ircListItem(s3, 
					       kSircConfig->colour_error, 
					       nicks);
	    irc->setWrapping(FALSE);
	    nicks->inSort(irc, TRUE);
	  }
	  else if(s3[0] == '+'){
	    s3.remove(0, 1);
	    ircListItem *irc = new ircListItem(s3, 
					       kSircConfig->colour_chan, 
					       nicks);
	    irc->setWrapping(FALSE);
	    nicks->inSort(irc);	    
	  }
	  else{
	    nicks->inSort(s3);
	  }
	  pos = string.find(" ", pos2, FALSE); // find next nick
	}
	nicks->setAutoUpdate(TRUE);         // clear and repaint the listbox
	nicks->repaint(TRUE);
	color = kSircConfig->colour_info;    // set to cyan colouring
	no_output = 1;
	break;
      case '<':
	string.remove(0, 2);                // clear junk
	pixmap = pix_greenp;                // For joins and leave use green
	color = kSircConfig->colour_chan;    // Pin gets for joins
	
	// Multiple type of parts, a signoff or a /part
	// Each get's get nick in a diffrent localtion
	// Set we search and find the nick and the remove it from the nick list
	// 1. /quit, signoff, nick after "^Singoff: "
	// 2. /part, leave the channek, nick after "has left \w+$"
	// 3. /kick, kicked off the channel, nick after "kicked off \w+$"
	//

	if(string.contains("Signoff: ")){   // Nick is right after the ": "
	  pos = string.find("Signoff: ") + 9;
	  s3 = string.mid(pos, string.find(' ', pos) - pos);
	}
	else if(string.contains("You have left channel ")){
	  pos = string.find("channel ", 0) + 8;
	  int end = string.length();
	  s3 = string.mid(pos, end - pos);
	  if(strcmp(channel_name, s3.data()) == 0){
	    no_output = 1;
	    string.truncate(0);
	    QApplication::postEvent(this, new QCloseEvent()); // WE'RE DEAD
	  }
	  s3 = "";
	}
	else if(string.contains("You have been kicked")){
	  switch(QMessageBox::information(this, "You have Been Kicked",
					  string.data() + 1, 
					  "Rejoin", "Leave", 0, 0, 1)){
	  case 0:
	    {
	      QString str = "/join " + QString(channel_name) + "\n";
	      emit outputLine(str);
	      if(ticker)
		ticker->show();
	      else
		this->show();
	    }
	    break;
	  case 1:
	    QApplication::postEvent(this, new QCloseEvent()); // WE'RE DEAD
	    break;
	  }
	  s3 = "";
	}
	else if(string.contains("has left")) // part
	  s3 = string.mid(1, string.find(' ', 1) - 1);
	else if(string.contains("kicked off")) // kick
	  s3 = string.mid(1, string.find(' ', 1) - 1);
	else if(string.contains("You have left"))
	  s3 = "";
	else{                                // uhoh, something we missed?
	  cerr << "String sucks: " << string << endl;
	  s3 = "";
	}
	no_output = 1;
	for(uint i = 0; i < nicks->count(); i++){ // Search and remove the nick
	  if(strcmp(s3, nicks->text(i)) == 0){
	    nicks->removeItem(i);
	    no_output = 0;
	  }
	}
	break;
      case '>':
	string.remove(0, 2);                   // remove junk 
	pixmap = pix_greenp;                   // set green pin
	color =   kSircConfig->colour_chan;     // set green
	if(string.contains("You have joined channel")){
	  int chan = string.findRev(" ", -1) + 1;
	  ASSERT(chan > 0);
	  s3 = string.mid(chan, string.length() - chan);
	  s3 = s3.lower();
	  this->show();
	  emit open_toplevel(s3);
	}
	else{
	  s3 = string.mid(1, string.find(' ', 1) - 1); // only 2 types of join
	  //	nicks->insertItem(s3, 0);      // add the sucker
	  nicks->inSort(s3);
	}
	break;
      case 'N':
	string.remove(0, 2);                   // remove the junk
	pixmap = pix_greenp;                   // set green pin
	color = kSircConfig->colour_chan;       // set freen
	s3 = string.mid(1, string.find(' ', 1) - 1); // find the old know
	pos = string.find("known as ") + 9;    // find the new nick
	s4 = string.mid(pos, string.length() - pos);
	//	cerr << s3 << "-" << s4 << endl;
	// search the list for the nick and remove it
	// since the list is source we should do a binary search...
	no_output = 1;            // don't display unless we find the nick
	for(uint i = 0; i < nicks->count(); i++){
	  if(strcmp(s3, nicks->text(i)) == 0){
	    no_output = 0;        // nick is in out list, so print the message
	    bool isOp = nicks->isTop(i); // Are they an op?
	    nicks->removeItem(i);        // remove old nick
	    if(isOp == TRUE){
	      ircListItem *irc = new ircListItem(s4, &red, nicks);
	      irc->setWrapping(FALSE);
	      nicks->inSort(irc, isOp);
	    }
	    else{
	      nicks->inSort(s4);     // add new nick in sorted poss
	                             // can't use changeItem since it
				     // doesn't maintain sort order
	    }
	  }
	}
	break;
      case ' ':
	string.remove(0, 1);      // * <something> use fancy * pixmap
	pixmap = pix_star;        // why? looks cool for dorks
	break;
      case '+':
	// Basic idea here is simple, go through the mode change and
	// assign each mode a + or a - and an argument or "" if there is
	// none.  After that each mode change it looked at to see if
	// we should handle it in any special way.  
	if(!string.contains("for user")){
	  pos = string.find("Mode change \"", 0);
	  if(pos > 0){
	    QStrList mode, arg;
	    char plus[] = "+";
	    pos += 13;
	    int endmode = string.find(" ", pos);
	    if(string[endmode-1] == '"')
	      endmode--;
	    int nextarg = endmode + 1;
	    for(; pos < endmode; pos++){
	      switch(string[pos]){
	      case '+':
	      case '-':
		plus[0] = string[pos];
		break;
	      case 'o':
	      case 'v':
	      case 'b':
	      case 'l':
	      case 'k':
		mode.append(plus + string.mid(pos, 1));
		{
		  int end = string.find(" ", nextarg);
		  if(end == -1){
		    cerr << "No arg: " << string << endl;
		    arg.append("");
		  }
		  else if(string[end-1] == '"')
		    end--;
		  arg.append(string.mid(nextarg, end - nextarg));
		  nextarg = end+1;
		}
		break;
	      default:
		mode.append(plus + string.mid(pos, 1));
		arg.append("");
	      }
	    }
	    // We have the modes set in mode and arg, now we go though
	    // looking at each mode seeing if we should handle it.
	    for(uint i = 0; i < mode.count(); i++){
	      if(strcasecmp(mode.at(i), "+o") == 0){
		for(uint j = 0; j < nicks->count(); j++){
		  if(strcmp(arg.at(i), nicks->text(j)) == 0){
		    nicks->setAutoUpdate(FALSE);
		    nicks->removeItem(j);           // remove old nick
		    ircListItem *irc = new ircListItem(arg.at(i), kSircConfig->colour_error, nicks);
		    irc->setWrapping(FALSE);
		    // add new nick in sorted pass,with colour
		    nicks->inSort(irc, TRUE);
		    nicks->setAutoUpdate(TRUE);
		    nicks->repaint();
		  }
		}
	      }
	      else if(strcasecmp(mode.at(i), "-o") == 0){
		for(uint j = 0; j < nicks->count(); j++){
		  if(strcmp(arg.at(i), nicks->text(j)) == 0){
		    nicks->setAutoUpdate(FALSE);
		    nicks->removeItem(j);     // remove old nick
		    nicks->inSort(arg.at(i)); // add new nick in sorted pass,with colour
		    nicks->setAutoUpdate(TRUE);
		    nicks->repaint();
		  }
		}
	      }
	      else if(strcasecmp(mode.at(i), "+v") == 0){
		for(uint j = 0; j < nicks->count(); j++){
		  if(strcmp(arg.at(i), nicks->text(j)) == 0){
		    nicks->setAutoUpdate(FALSE);
		    nicks->removeItem(j);           // remove old nick
		    ircListItem *irc = new ircListItem(arg.at(i), kSircConfig->colour_chan, nicks);
		    irc->setWrapping(FALSE);
		    // add new nick in sorted pass,with colour
		    nicks->inSort(irc);
		    nicks->setAutoUpdate(TRUE);
		    nicks->repaint();
		  }
		}
	      }
	      else if(strcasecmp(mode.at(i), "-v") == 0){
		for(uint j = 0; j < nicks->count(); j++){
		  if(strcmp(arg.at(i), nicks->text(j)) == 0){
		    nicks->setAutoUpdate(FALSE);
		    nicks->removeItem(j);     // remove old nick
		    nicks->inSort(arg.at(i)); // add new nick in sorted pass,with colour
		    nicks->setAutoUpdate(TRUE);
		    nicks->repaint();
		  }
		}
	      }
	      else{
		//	      cerr << "Did not handle: " << mode.at(i) << " arg: " << arg.at(i)<<endl;
	      }
	    }
	  }
	}
      default:
        string.remove(0, 3);      // by dflt remove junk, and use a ball
        pixmap = pix_bball;       // ball isn't used else where so we
	// can track down unkonws and add them
        color = kSircConfig->colour_info;
        //      cerr << "Unkoown control: " << c << endl;
      }
    }
    break;
  }

  if(no_output)                    // is no_output is null,return
				   // anull pointer
    return NULL;
  else                             // otherwise create a new IrcListItem...
    return new ircListItem(string,color,mainw,pixmap);

  return NULL; // make compiler happy or else it complans about
	       // getting to the end of a non-void func
}

void KSircTopLevel::UserSelected(int index)
{
  if(index >= 0)
    user_controls->popup(this->cursor().pos());
}

void KSircTopLevel::UserParseMenu(int id)
{
  if(nicks->currentItem() < 0){
    QMessageBox::warning(this, "Warning, dork at the helm Captain!\n",
			 "Warning, dork at the helm Captain!\nTry Selecting a nick first!");
    return;
  }
  if(strstr(user_menu->at(id)->action, "%s")){
    QMessageBox::warning(this, "%s no longer valid in action string",
			 "%s is deprecated, you MUST use $$dest_nick\n"
			 "intead.  Any valid sirc\n"
			 "variable may now be refrenced.\n"
			 "This includes repeated uses.\n\n"
			 "Options->Prefrences->User Menu to change it\n");
    return;
  }
  QString s;
  s = QString("/eval $dest_nick='") + 
    QString(nicks->text(nicks->currentItem())) + 
    QString("';\n");
  sirc_write(s);
  s = QString("/eval &docommand(eval{\"") + 
    QString(user_menu->at(id)->action) +
    QString("\"});\n");
  s.replace(QRegExp("\\$\\$"), "$");
  sirc_write(s);
}

void KSircTopLevel::UserUpdateMenu()
{
  int i = 0;
  UserControlMenu *ucm;
  //  QPopupMenu *umenu;

  user_controls->clear();
  for(ucm = user_menu->first(); ucm != 0; ucm = user_menu->next(), i++){
    if(ucm->type == UserControlMenu::Seperator){
      user_controls->insertSeparator();
    }
    else{
      user_controls->insertItem(ucm->title, i);
      if(ucm->accel)
	user_controls->setAccel(i, ucm->accel);
      if((ucm->op_only == TRUE) && (opami == FALSE))
	user_controls->setItemEnabled(i, FALSE);
    }
  }
  //  writePopUpMenu();
}

void KSircTopLevel::AccelScrollDownPage()
{
    mainw->pageDown();
}

void KSircTopLevel::AccelScrollUpPage()
{
    mainw->pageUp();
}
void KSircTopLevel::AccelPriorMsgNick()
{
  linee->setText(QString("/msg ") + nick_ring.current() + " ");

  if(nick_ring.at() > 0)
    nick_ring.prev();

}

void KSircTopLevel::AccelNextMsgNick()
{
  if(nick_ring.at() < ((int) nick_ring.count() - 1) )
    linee->setText(QString("/msg ") + nick_ring.next() + " ");
}

void KSircTopLevel::newWindow() 
{ 
  open_top *w = new open_top(); 
  connect(w, SIGNAL(open_toplevel(QString)),
	  this, SIGNAL(open_toplevel(QString)));
  w->show();
}

void KSircTopLevel::closeEvent(QCloseEvent *)
{
  // Let's not part the channel till we are acutally delete.
  // We should always get a close event, *BUT* we will always be deleted.
  //  if((channel_name[0] == '#') || (channel_name[0] == '&')){
  //    QString str = QString("/part ") + channel_name + "\n";
  //    emit outputLine(str);
  //  }

  //  hide();
  // Let's say we're closing, what ever connects to this should delete us.
  emit closing(this, channel_name); // This should call "delete this".
  // This line is NEVER reached.
}

void KSircTopLevel::resizeEvent(QResizeEvent *e)
{
  bool update = mainw->autoUpdate();
  mainw->setAutoUpdate(FALSE);
  KTopLevelWidget::resizeEvent(e);
//  cerr << "Updating list box\n";
  //  mainw->setTopItem(mainw->count()-1);
  if(mainw->maximumSize().width() > width()){
    mainw->setMinimumWidth(width() - 100);
  }
  pan->setAbsSeparatorPos(width()-100);
  emit changeSize();
  mainw->scrollToBottom();
  mainw->setAutoUpdate(update);
  emit changeSize();
  mainw->repaint(TRUE);
  repaint();
  mainw->scrollToBottom();

}

void KSircTopLevel::gotFocus()
{
  if(isVisible() == TRUE){
    if(have_focus == 0){
      if(channel_name[0] == '#'){
	QString str = "/join " + QString(channel_name) + "\n";
	emit outputLine(str);
      }
      have_focus = 1;
      emit currentWindow(this);
      //    cerr << channel_name << " got focusIn Event\n";
    }
  }
}

void KSircTopLevel::lostFocus()
{
  if(have_focus == 1){
    have_focus = 0;
    //    cerr << channel_name << " got focusIn Event\n";
  }

}

void KSircTopLevel::control_message(int command, QString str)
{
  switch(command){
  case CHANGE_CHANNEL: // 001 is defined as changeChannel
    emit changeChannel(channel_name, str.data());
    if(channel_name)
      delete channel_name;
    channel_name = qstrdup(str.data());
    have_focus = 0;
    setCaption(channel_name);
    mainw->scrollToBottom();
    break;
  case STOP_UPDATES:
    Buffer = TRUE;
    break;
  case RESUME_UPDATES:
    Buffer = FALSE;
    if(LineBuffer->isEmpty() == FALSE)
      sirc_receive(QString(""));
    break;
  case REREAD_CONFIG:
    mainw->setUpdatesEnabled(FALSE); // Let's try and reduce the flicker
    nicks->setUpdatesEnabled(FALSE); // just a little bit.
    linee->setUpdatesEnabled(FALSE);
    setUpdatesEnabled(FALSE);
    mainw->setFont(kSircConfig->defaultfont);
    nicks->setFont(kSircConfig->defaultfont);
    linee->setFont(kSircConfig->defaultfont);
    linee->resize(width(), linee->fontMetrics().lineSpacing() + 8);
    linee->setMinimumHeight(linee->fontMetrics().lineSpacing() + 8);
    resize(size()); // Make the Layout manager make everything fit right.
    //    emit changeSize();
    {
      QColorGroup cg = QColorGroup(*kSircConfig->colour_text, colorGroup().mid(), 
				   colorGroup().light(), colorGroup().dark(),
				   colorGroup().midlight(), 
				   *kSircConfig->colour_text, 
				   *kSircConfig->colour_background); 
      mainw->setPalette(QPalette(cg, cg, cg));
      nicks->setPalette(QPalette(cg, cg, cg));
      linee->setPalette(QPalette(cg, cg, cg));
    }
    UserUpdateMenu();  // Must call to update Popup.
    mainw->setUpdatesEnabled(TRUE);
    nicks->setUpdatesEnabled(TRUE);
    linee->setUpdatesEnabled(TRUE);
    emit changeSize(); // Have the ist box update correctly.
    setUpdatesEnabled(TRUE);
    repaint(TRUE);
    mainw->scrollToBottom();
    break;
  case SET_LAG:
    if(str.isNull() == FALSE){
      bool ok = TRUE;
      str.truncate(6);
      double lag = str.toDouble(&ok);
      if(ok == TRUE){
      	lag -= (lag*100.0 - int(lag*100.0))/100.0;
	lagmeter->display(lag);
      }
      else{
	lagmeter->display(str);
      }
    }
    break;
  default:
    cerr << "Unkown control message: " << str << endl;
  }
}

void KSircTopLevel::showTicker()
{
  myrect = geometry();
  mypoint = pos();
  ticker = new KSTicker(0, "ticker", WStyle_NormalBorder);
  ticker->setCaption(caption);
  kConfig->setGroup("TickerDefaults");
  ticker->setFont(kConfig->readFontEntry("font", new QFont("fixed")));
  ticker->setSpeed(kConfig->readNumEntry("tick", 30), 
		   kConfig->readNumEntry("step", 3));
  QColorGroup cg = QColorGroup(*kSircConfig->colour_text, colorGroup().mid(), 
                               colorGroup().light(), colorGroup().dark(),
                               colorGroup().midlight(), 
                               *kSircConfig->colour_text, 
                               *kSircConfig->colour_background); 
  ticker->setPalette(QPalette(cg,cg,cg));
  ticker->setBackgroundColor( *kSircConfig->colour_background );
  connect(ticker, SIGNAL(doubleClick()), 
	  this, SLOT(unHide()));
  connect(ticker, SIGNAL(closing()), 
	  this, SLOT(terminate()));
  this->hide();
  if(tickerrect.isEmpty() == FALSE){
    ticker->setGeometry(tickerrect);
    ticker->recreate(0, 0, tickerpoint, TRUE);
  }
  for(int i = 5; i > 0; i--)
    ticker->mergeString(QString(mainw->text(mainw->count()-i)) + " // ");
  
  ticker->show();
}

void KSircTopLevel::unHide()
{
  tickerrect = ticker->geometry();
  tickerpoint = ticker->pos();
  int tick, step;
  ticker->speed(&tick, &step);
  kConfig->setGroup("TickerDefaults");
  kConfig->writeEntry("font", ticker->font());
  kConfig->writeEntry("tick", tick);
  kConfig->writeEntry("step", step);
  kConfig->sync();
  delete ticker;
  ticker = 0;
  this->setGeometry(myrect);
  this->recreate(0, getWFlags(), mypoint, TRUE);
  this->show();
  linee->setFocus();  // Give SLE focus
}

QString KSircTopLevel::findNick(QString part, uint which = 0)
{
  QStrList matches;
  for(uint i=0; i < nicks->count(); i++){
    if(strlen(nicks->text(i)) >= part.length()){
      if(strnicmp(part, nicks->text(i), part.length()) == 0){
	matches.append(nicks->text(i));
      }
    }
  }
  if(matches.count() > 0){
    if(which < matches.count())
      return matches.at(which);
    else
      return 0;
  }
  return part;
    
}

void KSircTopLevel::openCutWindow()
{
  static KSCutDialog *kscd;
  if(!KSCutDialog::open){
    kscd = new KSCutDialog();
  }
  QString buffer;
  for(uint i = 0; i < mainw->count(); i++){
    buffer += mainw->text(i);
    buffer += "\n";
  }
  kscd->setText(buffer);
  kscd->show();
  kscd->scrollToBot();
}

void KSircTopLevel::pasteToWindow()
{
  QString text = kApp->clipboard()->text();
  text += "\n";
  if((text.contains("\n") > 4) || (text.length() > 300)){
      switch( QMessageBox::warning(this, "Large Paste Requested",
				   "You are about to paste a very \nlarge number of lines,\ndo you really want to do this?",
				   "Yes", "No", 0, 0, 1)){
      case 0:
	break;
      default:
	linee->setText("");
	return;
      }
  }
  if(text.contains("\n") > 1){
    linee->setUpdatesEnabled(FALSE);
    for(QString line = strtok(text.data(), "\n");
	line.isNull() == FALSE;
	line = strtok(NULL, "\n")){
      QString hold = linee->text();
      hold += line;
      linee->setText(hold);
      sirc_line_return();
      linee->setText("");
    }
    linee->setText("");
    linee->setUpdatesEnabled(TRUE);
    linee->update();
  }
  else{
    text.replace(QRegExp("\n"), "");
    QString line = linee->text();
    line += text;
    linee->setText(line);
  }
}

void KSircTopLevel::lineeTextChanged(const char *)
{
  tab_pressed = 0;
}
