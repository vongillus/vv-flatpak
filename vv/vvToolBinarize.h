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
#ifndef VVTOOLBINARIZE_H
#define VVTOOLBINARIZE_H
#include <QtDesigner/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvImageContour.h"
#include "ui_vvToolBinarize.h"
#include "clitkBinarizeImage_ggo.h"

//------------------------------------------------------------------------------
class vvToolBinarize:
  public vvToolWidgetBase,
  public vvToolBase<vvToolBinarize>, 
  private Ui::vvToolBinarize 
{
  Q_OBJECT
    public:
  vvToolBinarize(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolBinarize();

  //-----------------------------------------------------
  static void Initialize();
  void GetArgsInfoFromGUI();
  virtual void InputIsSelected(vvSlicerManager * m);

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  void valueChangedT1(double v);
  void valueChangedT2(double v);
  void UpdateSlice(int slicer,int slices);
  void enableLowerThan(bool b);
  void useFGBGtoggled(bool);
  void InteractiveDisplayToggled(bool b);

 protected:
  void RemoveVTKObjects();
  Ui::vvToolBinarize ui;
  args_info_clitkBinarizeImage mArgsInfo;
  std::vector<vvImageContour*> mImageContour;
  std::vector<vvImageContour*> mImageContourLower;
  bool mInteractiveDisplayIsEnabled;

}; // end class vvToolBinarize
//------------------------------------------------------------------------------

#endif

