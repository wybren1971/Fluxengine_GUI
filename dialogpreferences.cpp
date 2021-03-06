#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"
#include <QtWidgets>

QString numberofcommands;
bool boolFirsttime;

DialogPreferences::DialogPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPreferences)
{
    ui->setupUi(this);
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    m_fluxengine.setWorkingDirectory(settings.value("fluxengine").toString());

    if (settings.value("datalocation").toString() != "")
    {
        ui->cmbdatalocation->addItem(settings.value("datalocation").toString());
    }

    if (settings.value("fluxlocation").toString() != "")
    {
        ui->cmbfluxlocation->addItem(settings.value("fluxlocation").toString());
    }

    if (settings.value("fluxengine").toString() != "")
    {
        ui->cmbFluxengineLocation->addItem(settings.value("fluxengine").toString());
    }

    if (settings.value("csvlocation").toString() != "")
    {
        ui->cmbcsvlocation->addItem(settings.value("csvlocation").toString());
    }

    if (settings.value("showanalyzebutton").toString() != "")
    {
        ui->chkShowAnalyzeButton->setChecked(settings.value("showanalyzebutton").toBool());
    }

    if (settings.value("showinspectbutton").toString() != "")
    {
        ui->chkShowInspectButton->setChecked(settings.value("showinspectbutton").toBool());
    }

    numberofcommands = settings.value("NUMBER_OF_COMMANDS").toString();
    boolFirsttime = true;
    if (settings.value("NUMBER_OF_COMMANDS").toString() == "")
    {//set default
        ui->intNumberofcommands->setText("10");
    } else
    {
        ui->intNumberofcommands->setText(settings.value("NUMBER_OF_COMMANDS").toString());
    }


    QList<QRadioButton *> allRadioButtons = ui->groupBox_2->findChildren<QRadioButton*>();
    for (auto RadioButton : allRadioButtons) {
        if (settings.value("drive0").toString() == RadioButton->text())
        {
            RadioButton->setChecked(true);
        }
    }

    QList<QRadioButton *> allRadioButtons1 = ui->groupBox_3->findChildren<QRadioButton*>();
    for (auto RadioButton1 : allRadioButtons1) {
        if (settings.value("drive1").toString() == RadioButton1->text())
        {
            RadioButton1->setChecked(true);
        }
    }


    QValidator *validator = new QIntValidator(10, 99, this);
    ui->intNumberofcommands->setValidator(validator);

    connect(&m_fluxengine,&fluxengine::output,this,&DialogPreferences::output);
    connect(&m_fluxengine,&fluxengine::enableFluxengineCommands,this,&DialogPreferences::enablecommands);
    connect(ui->btndatalocation, SIGNAL(clicked()), SLOT(browse()));
    connect(ui->btncsvlocation, SIGNAL(clicked()), SLOT(browsecsv()));
    connect(ui->btnfluxlocation, SIGNAL(clicked()), SLOT(browseflux()));
    connect(ui->btnfluxenginelocation, SIGNAL(clicked()), SLOT(browsefluxengine()));
    connect(ui->btnInitialize, SIGNAL(clicked()), SLOT(initializefluxengine()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(save()));
    ui->tabWidget->setTabVisible(2, true);
    ui->tabWidget->setCurrentIndex(0);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(40); //near enough it gets updated when we now it precise
    ui->progressBar->setValue(0);
    ui->txtoutput->setVisible(true); //for debug purposes on windows
    readcounter = 0;
    writecounter = 0;
    State = 0;
    waitforfluzenginetofinish=false;
}

DialogPreferences::~DialogPreferences()
{
    delete ui;
}


void DialogPreferences::browsefluxengine()
{
   // QString directory;
    QDir directory;

    if (ui->cmbFluxengineLocation->count() == 0)
    {
        directory.setPath(QFileDialog::getOpenFileName(this,
                                tr("Set default fluxengine application"), QDir::currentPath()));
    } else
    {
        directory.setPath(QFileDialog::getOpenFileName(this,
                                tr("Set default fluxengine application"), ui->cmbFluxengineLocation->currentText()));
    }

    if (directory.absolutePath() != "") {
        if (ui->cmbFluxengineLocation->findText(directory.absolutePath()) == -1)
            ui->cmbFluxengineLocation->addItem(directory.absolutePath());
        ui->cmbFluxengineLocation->setCurrentIndex(ui->cmbFluxengineLocation->findText(directory.absolutePath()));
    }
}

void DialogPreferences::browseflux()
{
    QString directory;
    if (ui->cmbFluxengineLocation->count() == 0)
    {
        directory = QFileDialog::getExistingDirectory(this,
                                tr("Set default flux output directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    } else
    {
        directory = QFileDialog::getExistingDirectory(this,
                                tr("Set default flux output directory"), ui->cmbfluxlocation->currentText(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }

    if (!directory.isEmpty()) {
        if (ui->cmbfluxlocation->findText(directory) == -1)
            ui->cmbfluxlocation->addItem(directory);
        ui->cmbfluxlocation->setCurrentIndex(ui->cmbfluxlocation->findText(directory));
    }
}

void DialogPreferences::browsecsv()
{
    QString directory;
    if (ui->cmbcsvlocation->count() == 0)
    {
        directory = QFileDialog::getExistingDirectory(this,
                                tr("Set default csv output directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    } else
    {
        directory = QFileDialog::getExistingDirectory(this,
                                tr("Set default csv output directory"), ui->cmbcsvlocation->currentText(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }

    if (!directory.isEmpty()) {
        if (ui->cmbcsvlocation->findText(directory) == -1)
            ui->cmbcsvlocation->addItem(directory);
        ui->cmbcsvlocation->setCurrentIndex(ui->cmbcsvlocation->findText(directory));
    }
}

void DialogPreferences::browse()
{
    QString directory;
    if (ui->cmbFluxengineLocation->count() == 0)
    {
        directory = QFileDialog::getExistingDirectory(this,
                                tr("Set default data output directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    } else
    {
        directory = QFileDialog::getExistingDirectory(this,
                                tr("Set default data output directory"), ui->cmbdatalocation->currentText(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }

    if (!directory.isEmpty()) {
        if (ui->cmbdatalocation->findText(directory) == -1)
            ui->cmbdatalocation->addItem(directory);
        ui->cmbdatalocation->setCurrentIndex(ui->cmbdatalocation->findText(directory));
    }
}

void DialogPreferences::enablecommands(bool running)
{
//    qInfo() << Q_FUNC_INFO;
//    qInfo() << running;
    if (running)
    {
        waitforfluzenginetofinish = true;
    } else
    {
        waitforfluzenginetofinish = false;
    }
}

void DialogPreferences::initializefluxengine()
{
//    if (m_fluxengine.getWorkingDirectory() == "")
//    {

        if (ui->cmbFluxengineLocation->currentText()== "")
        {
            QString message;
            message = tr("Welcome to Fluxengine_GUI "
                         "first set the location of the fluxengine");

            QMessageBox::information(this, tr("Fluxengine_GUI Info"), message);
            return;
        } else
        {
            QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
            m_fluxengine.setWorkingDirectory(ui->cmbFluxengineLocation->currentText());
            settings.setValue("fluxengine", ui->cmbFluxengineLocation->currentText());  //we initialize this fluxengine binary so remember it
        }
//    }
    ui->btnInitialize->setEnabled(false);
    ui->buttonBox->setEnabled(false);
    this->setCursor(Qt::WaitCursor);
    //delete old values in registry
    //settings.beginGroup("readformatsdescription");
    //settings.beginGroup("readformats");
    //settings.beginGroup("readconfigs-" + readformats.at(readcounter));
    //settings.beginGroup("writeformatsdescription");
    //settings.beginGroup("writeformats");
    //settings.beginGroup("writeconfigs-" + writeformats.at(writecounter));
    //reset preferences read and write format to default
    if (boolFirsttime)
    {
        QStringList readformat;
        QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
        int i =0;
//        int j = 0;
//        qInfo() << Q_FUNC_INFO;
        settings.beginGroup("readformats");
        while (settings.value(QString::number(i)) != "")
        {
            if (settings.value(QString::number(i)).toString() == "")
            {
                break;
            } else
            {
                readformat.append(settings.value(QString::number(i)).toString());
    //            qInfo() << "Readformat " << settings.value(QString::number(i)).toString();
    //            qInfo() << "i: " << i;
                i++;
            }
        }
        settings.endGroup();
        if (readformat.size() > 0)
        {
        //    qInfo() << "size " << my_readformat.size();
            for (i=0;i < readformat.size();i++)
            {
               QString x = readformat[i];
                settings.beginGroup("readconfigs-" + x);
                settings.remove(""); //removes the group, and all it keys
                settings.endGroup();

            }
            settings.beginGroup("readformatsdescription");
            settings.remove(""); //removes the group, and all it keys
            settings.endGroup();
            settings.beginGroup("readformats");
            settings.remove(""); //removes the group, and all it keys
            settings.endGroup();
        }
        QStringList writeformat;
        i = 0;
    //    qInfo() << Q_FUNC_INFO;
        settings.beginGroup("writeformats");
        while (settings.value(QString::number(i)) != "")
        {
            if (settings.value(QString::number(i)).toString() == "")
            {
                break;
            } else
            {
                writeformat.append(settings.value(QString::number(i)).toString());
    //            qInfo() << "Readformat " << settings.value(QString::number(i)).toString();
    //            qInfo() << "i: " << i;
                i++;
            }
        }
        settings.endGroup();
        if (writeformat.size() > 0)
        {
        //    qInfo() << "size " << my_readformat.size();
            for (i=0;i < writeformat.size();i++)
            {
               QString x = writeformat[i];
                settings.beginGroup("writeconfigs-" + x);
                settings.remove(""); //removes the group, and all it keys
                settings.endGroup();

            }
            settings.beginGroup("writeformatsdescription");
            settings.remove(""); //removes the group, and all it keys
            settings.endGroup();
            settings.beginGroup("writeformats");
            settings.remove(""); //removes the group, and all it keys
            settings.endGroup();
        }
        settings.setValue("defaultreadformat", "");
        settings.setValue("defaultwriteformat", "");

        boolFirsttime = false;
    }
    if (!waitforfluzenginetofinish)
    {
        switch (State)
            {
            case 0:
            {
                m_fluxengine.setAddress("read");
//                qInfo() << State;
                m_fluxengine.start();
                waitforfluzenginetofinish = true;
                ui->progressBar->setValue(1);
                break;
            }
            case 1:
            {
                m_fluxengine.setAddress("write");
//                qInfo() << "address" << m_fluxengine.getAddress();
//                m_fluxengine.startdirect();
                m_fluxengine.start();
                waitforfluzenginetofinish = true;
                ui->progressBar->setValue(2);
                break;
            }
            case 2:
            {
                QString m_address = "read";
                int intTotal = readformats.size();
                int intTotalWrite = writeformats.size();
                if (readcounter < intTotal)
                {
                    m_fluxengine.setAddress(m_address + " " + readformats.at(readcounter) + " -C");
//                    qInfo() << State;
//                    m_fluxengine.startdirect();
                    m_fluxengine.start();
                    waitforfluzenginetofinish = true;
                    if ((intTotal +3 + intTotalWrite) > ui->progressBar->maximum())
                        ui->progressBar->setMaximum(intTotal +3 + intTotalWrite);
                    ui->progressBar->setValue(readcounter +3);
                }
                break;
            }
            case 3:
            {
                QString m_address = "write";
                int intTotal = writeformats.size();
                if (writecounter < intTotal)
                {
                    m_fluxengine.setAddress(m_address + " " + writeformats.at(writecounter) + " -C");
 //                   qInfo() << State;
//                    m_fluxengine.startdirect();
                    m_fluxengine.start();
                    waitforfluzenginetofinish = true;
                    ui->progressBar->setValue(readcounter + writecounter +3);
                }
                break;
            }
            case 4:
            {
                //ready
 //               qInfo() << "State" << State;
                ui->progressBar->setValue(ui->progressBar->maximum());
                this->setCursor(Qt::ArrowCursor);
                ui->btnInitialize->setEnabled(true);
                ui->buttonBox->setEnabled(true);
                //reset everything for next initialize
                boolFirsttime = true;
                State = 0;
                readcounter = 0;
                writecounter = 0;
                break;
            }
        default: //
            {
//                this->setCursor(Qt::ArrowCursor);
//                ui->btnInitialize->setEnabled(true);
//                ui->buttonBox->setEnabled(true);
//                boolFirsttime = true;
//                State = 0;
                break;
            }
        }
    }
}

QStringList DialogPreferences::initializeformats(QString data)
{
    QStringList Formats;
    //returns a stringlist with read or write formats.
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
//    qInfo() << data;

//    [readformats]
//    0=acornadfs: Acorn ADFS L/D/E/F 640kB/800kB/1600kB 3.5\" or 5.25\" 80-track double-sided
//    1=acorndfs: Acorn DFS 100kB/200kB 3.5\" or 5.25\" 40- or 80-track singled sided
//    10=eco1: VDS Eco1 1210kB 77-track mixed format double sided
//    11=f85: Durango F85 461kB 5.25\" 77-track single sided
//    12=fb100: Brother FB-100 100kB 3.5\" 40-track single-sided
//    13=ibm: PC 3.5\"/5.25\" any double sided format
//    14=macintosh: Macintosh 800kB 3.5\" GCR double-sided
//    15=micropolis: Micropolis MetaFloppy 630kB 5.25\" double-sided hard-sectored
//    16=mx: DVK MX 110kB/220kB/440kB 5.25\"
//    17=northstar: Northstar 87kB/175kB/350kB 5.25\" 35-track 10-sector hard sectored
//    18=tids990: Texas Instruments DS990 1126kB 8\" double-sided
//    19=victor9k: Victor 9000 / Sirius One 1224kB GCR variable sector double-sided
//    2=aeslanier: AES Lanier \"No Problem\" 616kB 5.25\" 77-track single sided hard sectored
//    20=zilogmcz: Zilog MCZ 320kB 8\" 77-track single-sided hard-sectored
//    3=amiga: Amiga 880kB 3.5\" double sided
//    4=ampro: Ampro 400kB/800kB 5.25\" 40/80 track double sided
//    5=apple2: Apple II 140kB DOS 3.3 5.25\" 40 track single sided
//    6=atarist: Atari ST any 3.5\" double sided
//    7=brother: Brother 120kB/240kB 3.5\" GCR
//    8=commodore1541: Commodore 1541 170kB 5.25\" GCR disks
//    9=commodore1581: Commodore 1581 800kB 3.5\" MFM disks

//    [writeformats]
//    0=amiga: Amiga 880kB 3.5\" double sided
//    1=atarist360: Atari ST 360kB 3.5\" 80-track 9-sector single sided
//    10=brother240: Brother 240kB 3.5\" 78-track GCR disks
//    11=commodore1541: Commodore 1541 170kB 5.25\" GCR disks
//    12=commodore1581: Commodore 1581 800kB 3.5\" MFM disks
//    13=hplif770: Hewlett-Packard LIF 770kB 3.5\" disks
//    14=ibm1200_525: PC 1200kB 5.25\" 80-track 15-sector double-sided
//    15=ibm1440: PC 1440kB 3.5\" 80-track 18-sector double-sided
//    16=ibm180_525: PC 180kB 5.25\" 40-track 9-sector single-sided
//    17=ibm360_525: PC 360kB 5.25\" 40-track 9-sector double-sided
//    18=ibm720: PC 720kB 3.5\" 80-track 9-sector double-sided
//    19=ibm720_525: PC 720kB 5.25\" 80-track 9-sector double-sided
//    2=atarist370: Atari ST 370kB 3.5\" 82-track 9-sector single sided
//    20=macintosh: Macintosh 800kB 3.5\" GCR double-sided
//    21=northstar175: Northstar 175kB 5.25\" 35-track single-sided double-density hard-sectored
//    22=northstar350: Northstar 350kB 5.25\" 35-track double-sided double-density hard-sectored
//    23=northstar87: Northstar 87.5kB 5.25\" 35-track single-sided single-density hard-sectored
//    24=tids990: Texas Instruments DS990 1126kB 8\" double-sided
//    3=atarist400: Atari ST 400kB 3.5\" 80-track 10-sector single sided
//    4=atarist410: Atari ST 410kB 3.5\" 82-track 10-sector single sided
//    5=atarist720: Atari ST 720kB 3.5\" 80-track 9-sector double sided
//    6=atarist740: Atari ST 740kB 3.5\" 82-track 9-sector double sided
//    7=atarist800: Atari ST 800kB 3.5\" 80-track 10-sector double sided
//    8=atarist820: Atari ST 820kB 3.5\" 82-track 10-sector double sided
//    9=brother120: Brother 120kB 3.5\" 39-track GCR disks
   if (data.contains("syntax: fluxengine read ") || (data.contains("syntax: fluxengine write ")))
   {
       //stop before: Available profile options include:
//       qInfo() << "Index " <<  data.indexOf("Available profile options include:");
       int i = 0;
        if (data.contains("fluxengine read "))
        {
            i = data.indexOf("fluxengine read ",0);
        } else
        {
            i = data.indexOf("fluxengine write ",0);
        }
        ui->txtoutput->appendPlainText("i: " + QString::number(i));
        int j = data.indexOf(":", i+1);
        ui->txtoutput->appendPlainText("j: " + QString::number(j));
        for (int x=0; x < data.size(); x++)
        {
            if(QSysInfo::productType() == "windows")
            {
                i = data.indexOf("\r", j);
                j = data.indexOf("\r", i+1);
            } else
            {
                i = data.indexOf("\n", j);
                j = data.indexOf("\n", i+1);
            }
            ui->txtoutput->appendPlainText("enter at position: " + QString::number(i));
//            qInfo() << data.mid(i, j+1 - i).trimmed();
//"acornadfs: Acorn ADFS L/D/E/F 640kB/800kB/1600kB 3.5\" or 5.25\" 80-track double-sided"
            QString typedescription = data.mid(i, j+1 - i).trimmed();
            int k = typedescription.indexOf(":",0);

            if (j == -1) break;
            //Dont add profile options
            if (j >= data.indexOf("Available profile options include:")) break;
            //append type
            int length = typedescription.size();
            Formats.append(typedescription.left(k).trimmed());
//            qInfo() << "type: " + typedescription.left(k).trimmed();
            Formats.append(typedescription.right(length - k-1).trimmed());
//            qInfo() << "description: " + typedescription.right(length - k-1).trimmed();

        }
   }
   return Formats;
}

QStringList DialogPreferences::getConfig(QString data)
{

//    encoder {
//      c64 {
//      }
//    }
//    decoder {
//      c64 {
//      }
//    }
//    cylinders {
//      start: 0
//      end: 79
//    }
//    heads {
//      start: 0
//      end: 0
//    }
//    comment: "Commodore 1541 170kB 5.25\" SS GCR"
//    image_writer {
//      filename: "commodore1541.img"
//      d64 {
//      }
//    }
//    image_reader {
//      filename: "commodore1541.img"
//      d64 {
//      }
//    }
    QStringList Configs;
    if (data.contains("encoder") || (data.contains("decoder")))
    {
        int i = data.indexOf(QRegExp("filename: "),0);
        int j;
        if(QSysInfo::productType() == "windows")
        {
            j = data.indexOf("\r", i+1);
        } else
        {
            j = data.indexOf("\n", i+1);
        }
        Configs.append(data.mid(i+11, j-12 - i).trimmed());
        i = data.indexOf(QRegExp("start: "),0);
        if(QSysInfo::productType() == "windows")
        {
            j = data.indexOf("\r", i+1);
        } else
        {
            j = data.indexOf("\n", i+1);
        }
        Configs.append(data.mid(i+7, j-7 - i).trimmed());
        i = data.indexOf(QRegExp("end: "),i);
        if(QSysInfo::productType() == "windows")
        {
            j = data.indexOf("\r", i+1);
        } else
        {
            j = data.indexOf("\n", i+1);
        }
        Configs.append(data.mid(i+5, j-5 - i).trimmed());
        i = data.indexOf(QRegExp("start: "),i);
        if(QSysInfo::productType() == "windows")
        {
            j = data.indexOf("\r", i+1);
        } else
        {
            j = data.indexOf("\n", i+1);
        }
        Configs.append(data.mid(i+7, j-7 - i).trimmed());
        i = data.indexOf(QRegExp("end: "),i);
        if(QSysInfo::productType() == "windows")
        {
            j = data.indexOf("\r", i+1);
        } else
        {
            j = data.indexOf("\n", i+1);
        }
        Configs.append(data.mid(i+5, j-5 - i).trimmed());
        return Configs;
    }
    //stringlist contains filename, start cylinder, stop cylinder, start Head, stop head.
    return Configs;
}

void DialogPreferences::output(QString data)
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    if (data.size() > 1)
    {
        ui->txtoutput->appendPlainText(data.trimmed());
    }
    if (data.contains("denied"))
    {
        State = 4;
    }
    int i = 0;
    int j = 0;
    if (data.trimmed() != "")
    {

        if (State == 0)
        {
            readformats = initializeformats(data);
            ui->txtoutput->appendPlainText("size:  "+ QString::number(readformats.size()));
            for (int l = 0; l < readformats.size() ;l++ )
            {
                QString x = readformats[l];
                if (i % 2)
                { //if odd then description
//                    qInfo() << "Odd" << i;
                    settings.beginGroup("readformatsdescription");
                    settings.setValue(QString::number(j), x);
                    settings.endGroup();
                } else
                { //if even then type
                    j = i/2;
                    settings.beginGroup("readformats");
                    settings.setValue(QString::number(j), x);
                    settings.endGroup();
                }
                i++;
            }
            State = 1;
            //save formats somewhere
        } else
        {
            if (State == 1)
            {
                writeformats = initializeformats(data);
                foreach (QString x, writeformats)
                {
//                    qInfo() << "j: " << j << "i: " << i;
                    if (i % 2)
                    { //if odd then description
    //                    qInfo() << "Odd" << i;
                        settings.beginGroup("writeformatsdescription");
                        settings.setValue(QString::number(j), x);
                        settings.endGroup();
                    } else
                    { //if even then type
                        j = i/2;
                        settings.beginGroup("writeformats");
                        settings.setValue(QString::number(j), x);
                        settings.endGroup();
                    }
                    i++;
                }
                State = 2;
                //save formats somewhere
            } else
            {
                if (State == 2)
                {
                    readconfigs = getConfig(data);

                    if (readcounter < readformats.size())
                    {
                        //types are add even
                        settings.beginGroup("readconfigs-" + readformats.at(readcounter));
                        foreach (QString x, readconfigs)
                        {
                            settings.setValue(QString::number(i), x);
                            i++;
                        }
                        settings.endGroup();
                        readcounter++;
                        readcounter++;
                    }
                    if (readcounter >= readformats.size())
                    {
                        State = 3;
                    }
//                    qInfo() << "State" << State;
//                    qInfo() << "readcounter" << readcounter;
//                    qInfo() << "readformats.size()" << readformats.size();
                 }   else
                    {
                        if (State == 3)
                        {
                            writeconfigs = getConfig(data);
                            if (writecounter < writeformats.size())
                            {
                                settings.beginGroup("writeconfigs-" + writeformats.at(writecounter));
                                foreach (QString x, writeconfigs)
                                {
                                    settings.setValue(QString::number(i), x);
                                    i++;
                                }
                                settings.endGroup();
                                writecounter++;
                                writecounter++;
                            }
                            if (writecounter >= writeformats.size())
                            {
                               State = 4;
                            }
                        }
//                        qInfo() << "State" << State;
//                        qInfo() << "writecounter" << writecounter;
//                        qInfo() << "writeformats.size()" << writeformats.size();
                    }
            }
        }
    }
    initializefluxengine();
}

void DialogPreferences::save()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
   //do save action
//    qInfo() << "save dialog preferences";
//    qInfo() << ui->cmbdatalocation->currentText();

    QList<QRadioButton *> allRadioButtons = ui->groupBox_2->findChildren<QRadioButton*>();
    for (auto RadioButton : allRadioButtons) {
        if (RadioButton->isChecked())
        {
            settings.setValue("drive0", RadioButton->text());
            if (RadioButton->text().contains("40"))
            {
                //40 drive track
                settings.setValue("drive040track", true);
            } else
            {
                //not 40 drive track
                settings.setValue("drive040track", false);
            }
        }
    }

    QList<QRadioButton *> allRadioButtons1 = ui->groupBox_3->findChildren<QRadioButton*>();
    for (auto RadioButton1 : allRadioButtons1) {
        if (RadioButton1->isChecked())
        {
            settings.setValue("drive1", RadioButton1->text());
            if (RadioButton1->text().contains("40"))
            {
                //40 drive track
                settings.setValue("drive140track", true);
            } else
            {
                //not 40 drive track
                settings.setValue("drive140track", false);
            }
        }
    }

    settings.setValue("datalocation", ui->cmbdatalocation->currentText());
    settings.setValue("fluxlocation", ui->cmbfluxlocation->currentText());
    settings.setValue("fluxengine", ui->cmbFluxengineLocation->currentText());
    settings.setValue("csvlocation", ui->cmbcsvlocation->currentText());
    settings.setValue("showanalyzebutton", ui->chkShowAnalyzeButton->isChecked());
    settings.setValue("showinspectbutton", ui->chkShowInspectButton->isChecked());
    settings.setValue("defaultreadformat", ui->cmbDefaultreadformat->currentIndex());
    settings.setValue("defaultwriteformat", ui->cmbDefaultwriteformat->currentIndex());
    if (ui->intNumberofcommands->text().toInt() < numberofcommands.toInt())
    {
        //Clear the remaining
        // 0 = command 1        new number of commands =2 so delete 0 en 1 en copy 2 to 0 en copy 3 to 1 en delete 2
        // 1 = command 2
        // 2 = command 3
        // 3 = command 4
        int i = numberofcommands.toInt() - ui->intNumberofcommands->text().toInt(); //4-2 = 2 = i
        int numberofnewcommands = ui->intNumberofcommands->text().toInt();
//        qInfo() << "i: " << i;
//        qInfo() << "numberofnewcommands: " << numberofnewcommands;

        for (int t = 0; t < i; t++)
        { //delete first commands
            settings.setValue("Fluxengine.command" + QString::number(t), "");
//            qInfo() << "t pass 1: " << t;
        }
        for (int t = 0; t < numberofnewcommands; t++)
        {   //copy old commands to new position
            settings.setValue("Fluxengine.command" + QString::number(t), settings.value("Fluxengine.command" + QString::number(t + i)));
//            qInfo() << "t pass 2: " << t;
        }
//        qInfo() << "numberofcommands.toInt() pass 3: " << numberofcommands.toInt();
        for (int t = numberofnewcommands; t<numberofcommands.toInt() ;t++ )
        {
            settings.remove("Fluxengine.command" + QString::number(t));
//             qInfo() << "t pass 3: " << t;
        }
    }
    settings.setValue("NUMBER_OF_COMMANDS", ui->intNumberofcommands->text());
}

void DialogPreferences::on_tabWidget_currentChanged(int index)
{//   qInfo() << index;
    if (index == 2)
    {
 //       qInfo() << ui->cmbDefaultreadformat->count();
        for (int i = ui->cmbDefaultreadformat->count(); i >= 0;i--)
        {
            ui->cmbDefaultreadformat->removeItem(i);
        }
//        qInfo() << ui->cmbDefaultwriteformat->count();
        for (int i = ui->cmbDefaultwriteformat->count(); i >= 0 ;i--)
        {
            ui->cmbDefaultwriteformat->removeItem(i);
        }
        QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
        QStringList readformat;
        int i =0;
        int j = 0;
    //    qInfo() << Q_FUNC_INFO;
        settings.beginGroup("readformats");
        while (settings.value(QString::number(i)) != "")
        {
            if (settings.value(QString::number(i)).toString() == "")
            {
                break;
            } else
            {
                readformat.append(settings.value(QString::number(i)).toString());
                if (settings.value(QString::number(i)).toString() == "ibm")
                    j = i;

    //            qInfo() << "Readformat " << settings.value(QString::number(i)).toString();
    //            qInfo() << "i: " << i;
                i++;
            }
        }
        settings.endGroup();
        settings.beginGroup("readformatsdescription");
        for (i = 0; i < readformat.size();i++)
        {
            ui->cmbDefaultreadformat->addItem(settings.value(QString::number(i)).toString());
        }
        settings.endGroup();
        if (settings.value("defaultreadformat").toString() == "")
            //not defined
        {
            ui->cmbDefaultreadformat->setCurrentIndex(j);
        } else
        {
            ui->cmbDefaultreadformat->setCurrentIndex(settings.value("defaultreadformat").toInt());
        }

        QStringList writeformat;
        i =0;
        j = 0;
    //    qInfo() << Q_FUNC_INFO;
        settings.beginGroup("writeformats");
        while (settings.value(QString::number(i)) != "")
        {
            if (settings.value(QString::number(i)).toString() == "")
            {
                break;
            } else
            {
                writeformat.append(settings.value(QString::number(i)).toString());
                if (settings.value(QString::number(i)).toString() == "ibm1440")
                    j = i;

    //            qInfo() << "Readformat " << settings.value(QString::number(i)).toString();
    //            qInfo() << "i: " << i;
                i++;
            }
        }
        settings.endGroup();
        settings.beginGroup("writeformatsdescription");
        for (i = 0; i < writeformat.size();i++)
        {
            ui->cmbDefaultwriteformat->addItem(settings.value(QString::number(i)).toString());
        }
        settings.endGroup();
        if (settings.value("defaultwriteformat").toString() == "")
            //not defined
        {
            ui->cmbDefaultwriteformat->setCurrentIndex(j);
        } else
        {
            ui->cmbDefaultwriteformat->setCurrentIndex(settings.value("defaultwriteformat").toInt());
        }
    }

}

