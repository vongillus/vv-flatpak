/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
#ifndef vvHelpDialog_h
#define vvHelpDialog_h
#include <iostream>
#include <vector>

#include "ui_vvHelpDialog.h"

class vvHelpDialog : public QDialog, private Ui::vvHelpDialog
{
    Q_OBJECT

public:
    vvHelpDialog() {
        setupUi(this);
    }
    ~vvHelpDialog() {}

public slots:

private:

};

#endif
