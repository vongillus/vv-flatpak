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
#ifndef VVDICOMSERIESSELECTOR_CXX
#define VVDICOMSERIESSELECTOR_CXX
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <gdcmFile.h>
#include <gdcmDocEntry.h>

#include "vvQDicomSeriesSelector.h"
//#include "vvUserConfig.h"

//====================================================================
vvDicomSeriesSelector::vvDicomSeriesSelector(QWidget* parent)
        :QDialog(parent) {
    // Set GUI
    ui.setupUi(this);
    // Correct GUI for splitter ...
    //   QSplitter * splitter = new QSplitter(this);
    //   splitter->setOrientation(Qt::Horizontal);
    //   ui.gridLayout1->addWidget(splitter, 0, 0, 1, 1);
    //   ui.mFrameLeft->setParent(splitter);
    //   ui.mFrameRight->setParent(splitter);
    //  ui.toolBox->setCurrentIndex(0);



    ui.mButtonBox->button(QDialogButtonBox::Open)->setEnabled(false);

    connect(ui.mBrowseButton, SIGNAL(released()),
            this, SLOT(BrowseButtonRelease()));
    connect(ui.mSearchButton, SIGNAL(released()),
            this, SLOT(SearchButtonRelease()));
    connect(ui.mListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(itemSelectionChanged()));
    connect(ui.mDicomDetailsListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(itemDetailsSelectionChanged()));

    // Initialization
    /*	if (config::get_current_path() != QString(0))
      mFoldername = config::get_current_path();
      else*/
    mFoldername = QFileInfo("./").absolutePath();

    mPreviousPath = mFoldername;
    ui.mFolderLineEdit->setText(mFoldername);
    //  ui.mTableWidget->setRowCount(0);
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::show() {
    QDialog::show();
    //grabKeyboard();
    // ui.mListWidget->grabKeyboard();
    //  ui.mDicomDetailsListWidget->grabKeyboard();
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::close() {
    QDialog::close();
    // ui.mListWidget->releaseKeyboard()
    //releaseKeyboard();
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::BrowseButtonRelease() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setFilter("DICOM files (*.dcm); All files (*)");
    mFoldername = dialog.getExistingDirectory(this,
                  "Select a folder to find DICOM image",
                  mPreviousPath);
    ui.mFolderLineEdit->setText(mFoldername);
    mPreviousPath = QFileInfo(mFoldername).absolutePath();
    //	config::set_current_path(mPreviousPath);
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::SearchButtonRelease() {
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails(true);
    //nameGenerator->SetDirectory(mFoldername.toStdString());
    nameGenerator->SetRecursive(ui.mIsRecursiveCheckBox->checkState() == Qt::Checked);

    //ds gérer recursive moi-meme pour progress ...
    nameGenerator->SetInputDirectory(mFoldername.toStdString());

    // insert in table
    typedef std::vector<std::string> SeriesIdContainer;
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

    for (unsigned int i=0; i<seriesUID.size(); i++) {
        // std::cout << seriesUID[i] << std::endl; //ds verif existe pas déja

        if (mListOfSeriesFilenames[seriesUID[i]]) {
            std::cout << seriesUID[i] << " exist" << std::endl;
        }
        else {
            // store filenames
            std::vector<std::string> * filenames = new std::vector<std::string>;
            const std::vector<std::string> & temp = nameGenerator->GetFileNames(seriesUID[i]);
            for (unsigned int j=0; j<temp.size(); j++) {
                filenames->push_back(temp[j]);
            }
            mListOfSeriesFilenames[seriesUID[i]] = filenames;

            // store first header
            gdcm::File *header = new gdcm::File();
            header->SetFileName((*filenames)[0]);
            header->SetMaxSizeLoadEntry(16384);
            header->SetLoadMode(gdcm::LD_NOSHADOW);// don't load shadow tags (in order to save memory)
            header->Load();


            //
            DD(header->GetEntryValue(0x0028,0x0030).c_str());

            //DS TEST DAVID
            DD(header->GetXSize());
            DD(header->GetYSize());
            DD(header->GetZSize());
            DD(header->GetXSpacing());
            DD(header->GetYSpacing());
            DD(header->GetZSpacing());
            DD(header->GetXOrigin());
            DD(header->GetYOrigin());
            DD(header->GetZOrigin());
            DD("\n");
            /*
              QString size = QString("%1x%2x%3")
              .arg(header->GetXSize())
              .arg(header->GetYSize())
              .arg(header->GetZSize());
              QString spacing = QString("%1x%2x%3")
              .arg(header->GetXSpacing())
              .arg(header->GetYSpacing())
              .arg(header->GetZSpacing());
              QString origin = QString("%1x%2x%3")
              .arg(header->GetXOrigin())
              .arg(header->GetYOrigin())
              .arg(header->GetZOrigin());
            */


            mDicomHeader[seriesUID[i]] = header;

            // new item
            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText(seriesUID[i].c_str());
            ui.mListWidget->insertItem(i, newItem);

            //AddSerieToTheTable(i, *filenames);
        }
    }
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::itemSelectionChanged() {
    //  mLabelSelected.setText(
    mCurrentSerie = ui.mListWidget->selectedItems()[0]->text().toStdString();
    mFilenames = mListOfSeriesFilenames[mCurrentSerie];
    ui.mButtonBox->button(QDialogButtonBox::Open)->setEnabled(true);

    if (mDicomInfo[mCurrentSerie] == "") {
        // 	QString m;
        // 	m = QString("Patient : <font color=\"blue\">%1</font><br>").arg(mDicomHeader[s]->GetEntryValue(0x0010,0x0010).c_str()); // Patient's name
        mDicomInfo[mCurrentSerie] = MakeDicomInfo(mCurrentSerie, mDicomHeader[mCurrentSerie]);
    }
    ui.mDicomInfoPanel->setText(mDicomInfo[mCurrentSerie]);

    // Detail tab
    ui.mDicomDetailsListWidget->clear();
    for (unsigned int i=0; i<mFilenames->size(); i++) {
        QListWidgetItem * newItem = new QListWidgetItem;
        newItem->setText(QFileInfo((*mFilenames)[i].c_str()).fileName());
        ui.mDicomDetailsListWidget->insertItem(i, newItem);
    }

}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::itemDetailsSelectionChanged() {
    unsigned int i = ui.mDicomDetailsListWidget->currentRow();
    if (i<mFilenames->size()) {
        if (mDicomDetails[(*mFilenames)[i]] == "") {
            std::ostringstream s;
            mDicomHeader[mCurrentSerie]->Print(s);

            QString l;
            gdcm::File * header = mDicomHeader[mCurrentSerie];
            gdcm::DocEntry * e = header->GetFirstEntry();
            while (e) {
                if (e->GetName() != "gdcm::Unknown") {
                    l += QString("%1 : %2\n")
                         .arg(e->GetName().c_str())
                         .arg((header->GetEntryValue(e->GetGroup(), e->GetElement())).c_str());
                }
                e = header->GetNextEntry();
            }

            mDicomDetails[(*mFilenames)[i]] = l.toStdString();
        }
        ui.mDicomDetailsLabel->setText(mDicomDetails[(*mFilenames)[i]].c_str());
    }
}
//====================================================================

//====================================================================
QString vvDicomSeriesSelector::MakeDicomInfo(std::string & s, gdcm::File *header) {
    QString n = QString("%1").arg(mListOfSeriesFilenames[s]->size());
    QString size = QString("%1x%2x%3")
                   .arg(header->GetXSize())
                   .arg(header->GetYSize())
                   .arg(header->GetZSize());
    QString spacing = QString("%1x%2x%3")
                      .arg(header->GetXSpacing())
                      .arg(header->GetYSpacing())
                      .arg(header->GetZSpacing());
    QString origin = QString("%1x%2x%3")
                     .arg(header->GetXOrigin())
                     .arg(header->GetYOrigin())
                     .arg(header->GetZOrigin());
    QString ss =
        //AddInfo(        "Serie ID   : ", s)+
        AddInfo(header, "Patient : ", 0x0010,0x0010)+
        AddInfo(        "Folder : ", QFileInfo((*mFilenames)[0].c_str()).canonicalPath().toStdString())+
        AddInfo(header, "Series Description : ", 0x0008,0x103e)+
        AddInfo(header, "Modality : ", 0x0008,0x0060)+
        AddInfo(header, "# images : ", 0x0020,0x0013)+
        AddInfo(        "# files : ", n.toStdString())+
        AddInfo(        "Size : ", size.toStdString())+
        AddInfo(        "Spacing : ", spacing.toStdString())+
        AddInfo(        "Origin : ", origin.toStdString())+
        AddInfo(header, "Pixel size : ", 0x0028,0x0100)+
        AddInfo(        "Pixel type : ", header->GetPixelType());
    return ss;
}
//====================================================================

//====================================================================
QString vvDicomSeriesSelector::AddInfo(gdcm::File *header, QString n, uint16_t group, uint16_t elem) {
    return AddInfo(n.toStdString(), header->GetEntryValue(group, elem));
}
//====================================================================

//====================================================================
QString vvDicomSeriesSelector::AddInfo(std::string n, std::string m) {
    QString s = QString("%1 <font color=\"blue\">%2</font><br>").
                arg(n.c_str()).arg(m.c_str());
    return s;
}
//====================================================================

//====================================================================
void vvDicomSeriesSelector::AddSerieToTheTable(int i, std::vector<std::string> & filenames) {
    gdcm::File *header = new gdcm::File();
    header->SetFileName(filenames[0]);
    header->SetMaxSizeLoadEntry(16384);
    header->SetLoadMode(gdcm::LD_NOSHADOW);// don't load shadow tags (in order to save memory)
    header->Load();
    //header->Print(cout);
    // ->GetValEntry
    //  mDicomHeader[] = header;

    /*

    QTableWidgetItem *newItem = new
    QTableWidgetItem(QString("# images = %1").arg(header->GetImageNumber()));
    //  newItem->setCheckState(Qt::Checked);
    //newItem->setFlags(!Qt::ItemIsEditable);
    DD(ui.mTableWidget->rowCount());
    ui.mTableWidget->setItem(i, 0, newItem);
    */
}
//====================================================================

#endif // VVDICOMSERIESSELECTOR_CXX
