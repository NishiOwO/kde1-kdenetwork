#ifndef PWSWIDGET_H 
#define PWSWIDGET_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <qwidget.h>
#include <qdict.h>

class QListView;
class QListViewItem;
class QWidgetStack;

class PWSWidget : public QWidget
{
    Q_OBJECT
public:
    PWSWidget(QWidget *parent = 0, const char *name = 0);
    virtual ~PWSWidget();
    void loadServers();
    void createGeneralPage();
    void createServerPage(const char *name);
    QListView *list;
    QWidgetStack *stack;
    int increaser;
    QDict <QWidget> pages;
public slots:
    void quit();
    void accept();
    void addServer();
    void restart();
    void flipPage(QListViewItem *item);
};

#endif // PWSWIDGET_H 