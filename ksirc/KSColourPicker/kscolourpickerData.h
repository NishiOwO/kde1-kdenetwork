/**********************************************************************

	--- Qt Architect generated file ---

	File: kscolourpickerData.h
	Last generated: Tue Jul 28 14:21:49 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef kscolourpickerData_included
#define kscolourpickerData_included

#include <qdialog.h>
#include <qpushbt.h>
#include <qlined.h>

class kscolourpickerData : public QDialog
{
    Q_OBJECT

public:

    kscolourpickerData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~kscolourpickerData();

public slots:


protected slots:


protected:
    QPushButton* PB_Pick;
    QPushButton* PB_Cancel;
    QLineEdit* SLE_SampleText;

};

#endif // kscolourpickerData_included
