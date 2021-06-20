#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"
#include <QtWidgets>

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
    ui->progressBar->setMaximum(5);
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
    qInfo() << Q_FUNC_INFO;
    qInfo() << running;
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
    if (m_fluxengine.getWorkingDirectory() == "")
    {
        if (ui->cmbFluxengineLocation->currentText()== "")
        {
            QString message;
            message = tr("Welcome to fluxengine_gui "
                         "first set the location of the fluxengine");

            QMessageBox::information(this, tr("Fluxengine Wizard Info"), message);
            return;
        } else
        {
            m_fluxengine.setWorkingDirectory(ui->cmbFluxengineLocation->currentText());
        }
    }

    if (!waitforfluzenginetofinish)
    {
        switch (State)
            {
            case 0:
            {
                m_fluxengine.setAddress("read");
                qInfo() << State;
                m_fluxengine.start();
                waitforfluzenginetofinish = true;
                ui->progressBar->setValue(1);
                break;
            }
            case 1:
            {
                m_fluxengine.setAddress("write");
                qInfo() << "address" << m_fluxengine.getAddress();
                m_fluxengine.start();
                waitforfluzenginetofinish = true;
                ui->progressBar->setValue(2);
                break;
            }
            case 2:
            {
                ui->progressBar->setValue(3);
                QString m_address = "read";
                int intTotal = readformats.size();
                if (readcounter < intTotal)
                {
                    m_fluxengine.setAddress(m_address + " " + readformats.at(readcounter) + " -C");
                    qInfo() << State;
                    m_fluxengine.start();
                    waitforfluzenginetofinish = true;
                    break;
                }
            }
            case 3:
            {
                ui->progressBar->setValue(4);
                QString m_address = "write";
                int intTotal = writeformats.size();
                if (writecounter < intTotal)
                {
                    m_fluxengine.setAddress(m_address + " " + writeformats.at(writecounter) + " -C");
                    qInfo() << State;
                    m_fluxengine.start();
                    waitforfluzenginetofinish = true;
                    break;
                }
            }
            case 4:
            {
                //ready
                ui->progressBar->setValue(5);
                break;
            }
        default: //
            {
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
    qInfo() << data;
   if (data.contains("syntax: fluxengine read ") || (data.contains("syntax: fluxengine write ")))
   {
        int i = data.indexOf(":",0);
        int j = data.indexOf(":", i+1);
        for (int x=0; x < data.count(); x++)
        {
            i = data.indexOf("\n", j);
            j = data.indexOf("\n", i+1);
            if (j == -1) break;
            Formats.append(data.mid(i, j+1 - i).trimmed());
        }
   }
   return Formats;
}

QStringList DialogPreferences::getConfig(QString data)
{
    QStringList Configs;
    if (data.contains("encoder") || (data.contains("decoder")))
    {
        int i = data.indexOf(":",0);
        int j;
            j = data.indexOf("\n", i+1);
            Configs.append(data.mid(i+3, j-4 - i).trimmed());
            i = data.indexOf(QRegExp("start: "),0);
            j = data.indexOf("\n", i+1);
            Configs.append(data.mid(i+7, j-7 - i).trimmed());
            i = data.indexOf(QRegExp("end: "),i);
            j = data.indexOf("\n", i+1);
            Configs.append(data.mid(i+5, j-5 - i).trimmed());
            i = data.indexOf(QRegExp("start: "),i);
            j = data.indexOf("\n", i+1);
            Configs.append(data.mid(i+7, j-7 - i).trimmed());
            i = data.indexOf(QRegExp("end: "),i);
            j = data.indexOf("\n", i+1);
            Configs.append(data.mid(i+5, j-5 - i).trimmed());
        return Configs;
    }
    //stringlist contains filename, start cylinder, stop cylinder, start Head, stop head.
    return Configs;
}

void DialogPreferences::output(QString data)
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    int i = 0;
    if (data.trimmed() != "")
    {

        if (State == 0)
        {
            readformats = initializeformats(data);
            settings.beginGroup("readformats");
            foreach (QString x, readformats)
            {
                settings.setValue(QString::number(i), x);
                i++;
            }
            settings.endGroup();
            State = 1;
            //save formats somewhere
        } else
        {
            if (State == 1)
            {
                writeformats = initializeformats(data);
                settings.beginGroup("writeformats");
                foreach (QString x, writeformats)
                {
                    settings.setValue(QString::number(i), x);
                    i++;
                }
                settings.endGroup();
                State = 2;
                //save formats somewhere
            } else
            {
                if (State == 2)
                {
                    readconfigs = getConfig(data);
                    if (readcounter < readformats.size())
                    {
                        settings.beginGroup("readconfigs-" + readformats.at(readcounter));
                        foreach (QString x, readconfigs)
                        {
                            settings.setValue(QString::number(i), x);
                            i++;
                        }
                        settings.endGroup();
                        readcounter++;
                    } else {
                        State = 3;
                    }
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
                             } else
                            {
                               State = 4;
                            }
                        }
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
    settings.setValue("NUMBER_OF_COMMANDS", ui->intNumberofcommands->text());
}
