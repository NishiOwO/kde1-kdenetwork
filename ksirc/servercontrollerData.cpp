/**********************************************************************

	--- Qt Architect generated file ---

	File: servercontrollerData.cpp
	Last generated: Wed Dec 17 23:33:11 1997

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include "servercontrollerData.h"

#define Inherited KSircControl

#include <qlabel.h>
#include <qlayout.h>

servercontrollerData::servercontrollerData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, 233472 )
{
	BaseBorder = new QFrame( this, "Frame_2" );
	BaseBorder->setGeometry( 5, 35, 400, 160 );
	BaseBorder->setMinimumSize( 10, 10 );
	BaseBorder->setMaximumSize( 32767, 32767 );
	BaseBorder->setFrameStyle( 33 );

	QLabel* dlgedit_Label_1;
	dlgedit_Label_1 = new QLabel( this, "Label_1" );
	dlgedit_Label_1->setGeometry( 20, 40, 370, 20 );
	dlgedit_Label_1->setMinimumSize( 10, 10 );
	dlgedit_Label_1->setMaximumSize( 32767, 32767 );
	{
		QColorGroup normal( QColor( QRgb(0) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(128) ), QColor( QRgb(16777215) ) );
		QColorGroup disabled( QColor( QRgb(8421504) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
		QColorGroup active( QColor( QRgb(0) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
		QPalette palette( normal, disabled, active );
		dlgedit_Label_1->setPalette( palette );
	}
	{
		QFont font( "helvetica", 12, 75, 0 );
		font.setStyleHint( (QFont::StyleHint)0 );
		font.setCharSet( (QFont::CharSet)0 );
		dlgedit_Label_1->setFont( font );
	}
	dlgedit_Label_1->setText( "Active Server Connections" );
	dlgedit_Label_1->setAlignment( 289 );
	dlgedit_Label_1->setMargin( -1 );

	MenuBar = new KMenuBar( this, "User_2" );
	MenuBar->setGeometry( 0, 0, 410, 30 );
	MenuBar->setMinimumSize( 10, 10 );
	MenuBar->setMaximumSize( 32767, 32767 );

	ConnectionTree = new KTreeList( this, "User_3" );
	ConnectionTree->setGeometry( 20, 60, 370, 120 );
	ConnectionTree->setMinimumSize( 10, 10 );
	ConnectionTree->setMaximumSize( 32767, 32767 );

	QBoxLayout* dlgedit_layout_1 = new QBoxLayout( this, QBoxLayout::TopToBottom, 5, 5, NULL );
	dlgedit_layout_1->addStrut( 0 );

	resize( 410,200 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


servercontrollerData::~servercontrollerData()
{
}
