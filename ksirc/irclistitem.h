#ifndef IRCLISTITEM_H
#define IRCLISTITEM_H

#include <qobject.h>
#include <qcolor.h>  
#include <qlistbox.h>
#include <qpainter.h>  
#include <qpixmap.h>
#include <qstrlist.h>


class ircListItem : public QObject, 
		    public QListBoxItem
{
 Q_OBJECT;
 public:
  ircListItem(QString s, const QColor *c, QListBox *lb, QPixmap *p=0);
  ~ircListItem();

  virtual int row();

  QString getText(){
    return itext;
  }

  void setWrapping(bool);
  bool wrapping();

public slots:
  virtual void updateSize();
  
protected:
  virtual void paint(QPainter *);
  virtual int height ( const QListBox * ) const;
  virtual int width ( const QListBox * ) const;  
  
  virtual void setupPainterText();

  virtual void colourDrawText(QPainter *p, int startx, int starty, char *str);

private:

  QPixmap *pm;
  const QColor *colour;
  QString itext;

  int rows;

  int linewidth;
  int lineheight;

  QStrList *paint_text;
  QListBox *parent_lb;
  int yPos;
  int xPos;

  bool Wrapping;

  static const int maxcolour;
  static const QColor num2colour[16];
  

};

#endif
