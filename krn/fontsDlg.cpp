#include "fontsDlg.h"

#define Inherited QDialog

#include <kapp.h>
#include <kconfig.h>
#include <qfont.h>
#include <qstrlist.h>
#include <X11/Xlib.h>


#include "tlform.h"
#include "typelayout.h"

#include "fontsDlg.moc"

//Prototypes for utility functions
void addFont( QStrList &list, const char *xfont );
void getFontList( QStrList &list, const char *pattern );


extern KConfig *conf;

fontsDlg::fontsDlg(QWidget* parent,const char* name):Inherited( parent, name, TRUE )
{



    TLForm *f=new TLForm("expiration",
                         klocale->translate("Message Display Settings"),
                         this);

    KTypeLayout *l=f->layout;

    l->addGroup("entries","",true);
    
    l->addLabel("l1",klocale->translate("Font Size:"));
    fontSize=(QComboBox *)(l->addComboBox("fontSize")->widget);
    l->newLine();
    l->addLabel("l2", klocale->translate("Standard Font"));
    stdFontName=(QComboBox *)(l->addComboBox("stdFontName")->widget);
    l->newLine();
    l->skip();
    samp1=(QLineEdit *)(l->addLineEdit("samp1",
                                       conf->readEntry("StandardFontTest",
                                                       QString("Standard Font Test").data()))->widget);
    l->newLine();
    l->addLabel("l3", klocale->translate("Fixed Font"));
    fixedFontName=(QComboBox *)(l->addComboBox("fixedFontName")->widget);
    l->newLine();
    l->skip();
    samp2=(QLineEdit *)(l->addLineEdit("samp2",
                                       conf->readEntry("FixedFontTest",QString("Fixed Font Test").data()))->widget);
    l->newLine();
    
    l->addLabel("l4", klocale->translate("Background Color"));
    bgColor=(KColorButton *)(l->addColorButton("bgColor")->widget);
    l->newLine();
    l->addLabel("l5", klocale->translate("Foreground Color"));
    fgColor=(KColorButton *)(l->addColorButton("fgColor")->widget);
    l->newLine();
    l->addLabel("l6", klocale->translate("Link Color"));
    linkColor=(KColorButton *)(l->addColorButton("linkColor")->widget);
    l->newLine();
    l->addLabel("l7", klocale->translate("Followed Color"));
    followColor=(KColorButton *)(l->addColorButton("followColor")->widget);
    l->endGroup();

    l->newLine();

    l->addGroup("buttons","",false);
    b1=(QPushButton *)(l->addButton("b1",klocale->translate("OK"))->widget);
    b2=(QPushButton *)(l->addButton("b2",klocale->translate("Cancel"))->widget);
    l->endGroup();

    l->activate();
    
    fontSize->insertItem(klocale->translate( "Small") );
    fontSize->insertItem(klocale->translate( "Normal" ));
    fontSize->insertItem(klocale->translate( "Large" ));
    fontSize->insertItem(klocale->translate( "Huge" ));


    connect (b1,SIGNAL(clicked()),this,SLOT(accept()));
    connect (b1,SIGNAL(clicked()),this,SLOT(save()));
    connect (b2,SIGNAL(clicked()),this,SLOT(reject()));
    conf->setGroup("ArticleListOptions");
    fontSize->setCurrentItem(conf->readNumEntry("DefaultFontBase",3)-2);
    QStrList stdfl,fixedfl;
    getFontList( stdfl, "-*-*-*-*-*-*-*-*-*-*-p-*-*-*" );
    //I add these, because some people may prefer all-fixed width fonts
    //(for ascii art, I suppose)
    getFontList( stdfl, "-*-*-*-*-*-*-*-*-*-*-m-*-*-*" );
    stdFontName->insertStrList(&stdfl);
    getFontList( fixedfl, "-*-*-*-*-*-*-*-*-*-*-m-*-*-*" );
    fixedFontName->insertStrList(&fixedfl);

    connect (fixedFontName,SIGNAL(activated(int)),this,SLOT(syncFonts(int)));
    connect (stdFontName,SIGNAL(activated(int)),this,SLOT(syncFonts(int)));
    stdFontName->setCurrentItem(stdfl.find
                                (conf->readEntry("StandardFont",QString("helvetica").data())));
    fixedFontName->setCurrentItem(fixedfl.find
                                  (conf->readEntry("FixedFont",QString("courier").data())));
    syncFonts(0);
    QColor c;
    c=QColor("white");
    bgColor->setColor(conf->readColorEntry("BackgroundColor",&c));
    c=QColor("black");
    fgColor->setColor(conf->readColorEntry("ForegroundColor",&c));
    c=QColor("blue");
    linkColor->setColor(conf->readColorEntry("LinkColor",&c));
    c=QColor("red");
    followColor->setColor(conf->readColorEntry("FollowedColor",&c));
}


void fontsDlg::syncFonts(int)
{
    samp1->setFont(QFont(stdFontName->currentText()));
    samp2->setFont(QFont(fixedFontName->currentText()));
}

fontsDlg::~fontsDlg()
{
}

void fontsDlg::save()
{
    conf->setGroup("ArticleListOptions");
    conf->writeEntry ("DefaultFontBase",fontSize->currentItem()+2);
    conf->writeEntry ("StandardFont",stdFontName->currentText());
    conf->writeEntry ("FixedFont",fixedFontName->currentText());
    conf->writeEntry ("StandardFontTest",samp1->text());
    conf->writeEntry ("FixedFontTest",samp2->text());
    conf->writeEntry ("BackgroundColor",bgColor->color());
    conf->writeEntry ("ForegroundColor",fgColor->color());
    conf->writeEntry ("LinkColor",linkColor->color());
    conf->writeEntry ("FollowedColor",followColor->color());
    conf->sync();
}

void addFont( QStrList &list, const char *xfont )
{
        const char *ptr = strchr( xfont, '-' );
        if ( !ptr )
                return;

        ptr = strchr( ptr + 1, '-' );
        if ( !ptr )
                return;

        QString font = ptr + 1;

        int pos;
        if ( ( pos = font.find( '-' ) ) > 0 )
        {
                font.truncate( pos );

                if ( font.find( "open look", 0, false ) >= 0 )
                        return;


                QStrListIterator it( list );

                for ( ; it.current(); ++it )
                        if ( it.current() == font )
                                return;

                list.inSort( font );
        }
}


void getFontList( QStrList &list, const char *pattern )
{
        int num;

        char **xFonts = XListFonts( qt_xdisplay(), pattern, 200, &num );

        for ( int i = 0; i < num; i++ )
        {
                addFont( list, xFonts[i] );
        }

        XFreeFontNames( xFonts );
}
