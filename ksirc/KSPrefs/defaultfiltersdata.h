/**********************************************************************

	--- Qt Architect generated file ---

	File: defaultfiltersdata.h
	Last generated: Tue Feb 17 09:36:02 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef defaultfiltersdata_included
#define defaultfiltersdata_included

#include <qframe.h>
#include <qlabel.h>
#include "kspinbox.h"
#include <qchkbox.h>

class defaultfiltersdata : public QFrame
{
    Q_OBJECT

public:

    defaultfiltersdata
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~defaultfiltersdata();

public slots:


protected slots:


protected:
    QCheckBox* kSircColours;
    QCheckBox* mircColours;
    KNumericSpinBox* NickFColour;
    KNumericSpinBox* NickBColour;
    QLabel* LabelNickB;
    KNumericSpinBox* usHighlightColour;

};

#endif // defaultfiltersdata_included
