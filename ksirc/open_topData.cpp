/**********************************************************************

	--- Qt Architect generated file ---

	File: open_topData.cpp
	Last generated: Thu Dec 4 21:25:20 1997

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include "open_topData.h"

#define Inherited QDialog

#include <qlabel.h>
#include <qpushbt.h>

open_topData::open_topData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, 45056 )
{
	QLabel* dlgedit_Label_1;
	dlgedit_Label_1 = new QLabel( this, "Label_1" );
	dlgedit_Label_1->setGeometry( 10, 10, 100, 30 );
	dlgedit_Label_1->setMinimumSize( 10, 10 );
	dlgedit_Label_1->setMaximumSize( 32767, 32767 );
	{
		QFont font( "helvetica", 12, 63, 0 );
		font.setStyleHint( (QFont::StyleHint)0 );
		font.setCharSet( (QFont::CharSet)0 );
		dlgedit_Label_1->setFont( font );
	}
	dlgedit_Label_1->setText( i18n("Channel/Nick:") );
	dlgedit_Label_1->setAlignment( 33 );
	dlgedit_Label_1->setMargin( -1 );

	okButton = new QPushButton( this, "PushButton_1" );
	okButton->setGeometry( 200, 50, 190, 30 );
	okButton->setMinimumSize( 10, 10 );
	okButton->setMaximumSize( 32767, 32767 );
	{
		QFont font( "helvetica", 12, 63, 0 );
		font.setStyleHint( (QFont::StyleHint)0 );
		font.setCharSet( (QFont::CharSet)0 );
		okButton->setFont( font );
	}
	connect( okButton, SIGNAL(clicked()), SLOT(create_toplevel()) );
	okButton->setText( i18n("Ok") );
	okButton->setAutoRepeat( FALSE );
	okButton->setAutoResize( FALSE );
	okButton->setAutoDefault( TRUE );

	QPushButton* dlgedit_PushButton_2;
	dlgedit_PushButton_2 = new QPushButton( this, "PushButton_2" );
	dlgedit_PushButton_2->setGeometry( 10, 50, 180, 30 );
	dlgedit_PushButton_2->setMinimumSize( 10, 10 );
	dlgedit_PushButton_2->setMaximumSize( 32767, 32767 );
	connect( dlgedit_PushButton_2, SIGNAL(clicked()), SLOT(terminate()) );
	dlgedit_PushButton_2->setText( i18n("Cancel") );
	dlgedit_PushButton_2->setAutoRepeat( FALSE );
	dlgedit_PushButton_2->setAutoResize( FALSE );

	nameSLE = new QComboBox( TRUE, this, "ComboBox_2" );
	nameSLE->setGeometry( 100, 10, 290, 30 );
	nameSLE->setMinimumSize( 10, 10 );
	nameSLE->setMaximumSize( 32767, 32767 );
	nameSLE->setInsertionPolicy( QComboBox::NoInsertion );
	nameSLE->setSizeLimit( 10 );
	nameSLE->setAutoResize( FALSE );

	resize( 400,90 );
	setMinimumSize( 400, 90 );
	setMaximumSize( 400, 90 );
}


open_topData::~open_topData()
{
}
void open_topData::create_toplevel()
{
}
void open_topData::terminate()
{
}
