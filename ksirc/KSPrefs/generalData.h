/**********************************************************************

	--- Qt Architect generated file ---

	File: generalData.h
	Last generated: Sun Oct 18 14:12:50 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef generalData_included
#define generalData_included

#include <qwidget.h>
#include <qchkbox.h>
#include <qpushbt.h>
#include <qlined.h>

class generalData : public QWidget
{
    Q_OBJECT

public:

    generalData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~generalData();

public slots:


protected slots:


protected:
    QCheckBox* CB_AutoCreateWin;
    QCheckBox* CB_BeepNotify;
    QCheckBox* CB_NickCompletion;
    QCheckBox* CB_ColourPicker;
    QCheckBox* CB_AutoRejoin;
    QCheckBox* CB_BackgroundPix;
    QLineEdit* SLE_BackgroundFile;
    QPushButton* PB_BackgroundBrowse;
    QCheckBox* CB_DisplayTopic;

};

#endif // generalData_included
