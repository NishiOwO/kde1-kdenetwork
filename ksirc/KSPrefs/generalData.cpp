/**********************************************************************

	--- Qt Architect generated file ---

	File: generalData.cpp
	Last generated: Sun Jul 26 19:55:04 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include "generalData.h"

#define Inherited QWidget

#include <qbttngrp.h>

generalData::generalData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, 0 )
{
	QButtonGroup* dlgedit_ButtonGroup_4;
	dlgedit_ButtonGroup_4 = new QButtonGroup( this, "ButtonGroup_4" );
	dlgedit_ButtonGroup_4->setGeometry( 10, 10, 380, 110 );
	dlgedit_ButtonGroup_4->setMinimumSize( 10, 10 );
	dlgedit_ButtonGroup_4->setMaximumSize( 32767, 32767 );
	dlgedit_ButtonGroup_4->setFrameStyle( 49 );
	dlgedit_ButtonGroup_4->setTitle( "" );
	dlgedit_ButtonGroup_4->setAlignment( 1 );

	QButtonGroup* dlgedit_ButtonGroup_3;
	dlgedit_ButtonGroup_3 = new QButtonGroup( this, "ButtonGroup_3" );
	dlgedit_ButtonGroup_3->setGeometry( 10, 160, 380, 130 );
	dlgedit_ButtonGroup_3->setMinimumSize( 10, 10 );
	dlgedit_ButtonGroup_3->setMaximumSize( 32767, 32767 );
	dlgedit_ButtonGroup_3->setFrameStyle( 49 );
	dlgedit_ButtonGroup_3->setTitle( "" );
	dlgedit_ButtonGroup_3->setAlignment( 1 );

	CB_AutoCreateWin = new QCheckBox( this, "CheckBox_1" );
	CB_AutoCreateWin->setGeometry( 20, 20, 170, 30 );
	CB_AutoCreateWin->setMinimumSize( 10, 10 );
	CB_AutoCreateWin->setMaximumSize( 32767, 32767 );
	CB_AutoCreateWin->setText( i18n("Auto Create Windows") );
	CB_AutoCreateWin->setAutoRepeat( FALSE );
	CB_AutoCreateWin->setAutoResize( FALSE );
	CB_AutoCreateWin->setChecked( TRUE );

	CB_BeepNotify = new QCheckBox( this, "CheckBox_3" );
	CB_BeepNotify->setGeometry( 210, 20, 170, 30 );
	CB_BeepNotify->setMinimumSize( 10, 10 );
	CB_BeepNotify->setMaximumSize( 32767, 32767 );
	CB_BeepNotify->setText( i18n("Beep on Notify") );
	CB_BeepNotify->setAutoRepeat( FALSE );
	CB_BeepNotify->setAutoResize( FALSE );
	CB_BeepNotify->setChecked( TRUE );

	CB_NickCompletion = new QCheckBox( this, "CheckBox_4" );
	CB_NickCompletion->setGeometry( 20, 50, 170, 30 );
	CB_NickCompletion->setMinimumSize( 10, 10 );
	CB_NickCompletion->setMaximumSize( 32767, 32767 );
	CB_NickCompletion->setText( i18n("Nick Completion") );
	CB_NickCompletion->setAutoRepeat( FALSE );
	CB_NickCompletion->setAutoResize( FALSE );
	CB_NickCompletion->setChecked( TRUE );

	CB_ColourPicker = new QCheckBox( this, "CheckBox_5" );
	CB_ColourPicker->setGeometry( 210, 50, 170, 30 );
	CB_ColourPicker->setMinimumSize( 10, 10 );
	CB_ColourPicker->setMaximumSize( 32767, 32767 );
	CB_ColourPicker->setText( i18n("Colour Picker Popup") );
	CB_ColourPicker->setAutoRepeat( FALSE );
	CB_ColourPicker->setAutoResize( FALSE );
	CB_ColourPicker->setChecked( TRUE );

	CB_AutoRejoin = new QCheckBox( this, "CheckBox_6" );
	CB_AutoRejoin->setGeometry( 20, 80, 170, 30 );
	CB_AutoRejoin->setMinimumSize( 10, 10 );
	CB_AutoRejoin->setMaximumSize( 32767, 32767 );
	CB_AutoRejoin->setText( i18n("Auto Rejoin") );
	CB_AutoRejoin->setAutoRepeat( FALSE );
	CB_AutoRejoin->setAutoResize( FALSE );
	CB_AutoRejoin->setChecked( TRUE );

	CB_BackgroundPix = new QCheckBox( this, "CheckBox_8" );
	CB_BackgroundPix->setGeometry( 20, 180, 190, 30 );
	CB_BackgroundPix->setMinimumSize( 10, 10 );
	CB_BackgroundPix->setMaximumSize( 32767, 32767 );
	CB_BackgroundPix->setText( i18n("Enable Background Pixmap") );
	CB_BackgroundPix->setAutoRepeat( FALSE );
	CB_BackgroundPix->setAutoResize( FALSE );

	SLE_BackgroundFile = new QLineEdit( this, "LineEdit_6" );
	SLE_BackgroundFile->setGeometry( 20, 220, 260, 30 );
	SLE_BackgroundFile->setMinimumSize( 10, 10 );
	SLE_BackgroundFile->setMaximumSize( 32767, 32767 );
	SLE_BackgroundFile->setText( "" );
	SLE_BackgroundFile->setMaxLength( 32767 );
	SLE_BackgroundFile->setEchoMode( QLineEdit::Normal );
	SLE_BackgroundFile->setFrame( TRUE );

	PB_BackgroundBrowse = new QPushButton( this, "PushButton_7" );
	PB_BackgroundBrowse->setGeometry( 290, 220, 90, 30 );
	PB_BackgroundBrowse->setMinimumSize( 10, 10 );
	PB_BackgroundBrowse->setMaximumSize( 32767, 32767 );
	PB_BackgroundBrowse->setText( i18n("Browse") );
	PB_BackgroundBrowse->setAutoRepeat( FALSE );
	PB_BackgroundBrowse->setAutoResize( FALSE );

	dlgedit_ButtonGroup_4->insert( CB_AutoCreateWin );
	dlgedit_ButtonGroup_4->insert( CB_BeepNotify );
	dlgedit_ButtonGroup_4->insert( CB_NickCompletion );
	dlgedit_ButtonGroup_4->insert( CB_ColourPicker );
	dlgedit_ButtonGroup_4->insert( CB_AutoRejoin );

	dlgedit_ButtonGroup_3->insert( CB_BackgroundPix );
	dlgedit_ButtonGroup_3->insert( PB_BackgroundBrowse );

	resize( 400,300 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


generalData::~generalData()
{
}
