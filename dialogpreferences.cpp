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
    State = 0;
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
        //
    }
}
void DialogPreferences::initializefluxengine()
{
    qInfo() << Q_FUNC_INFO;
    qInfo() << State;
    switch (State) {
        case 0:
            m_fluxengine.setAddress("read");
            m_fluxengine.start();
            break;
        case 1:
//           if (m_fluxengine.busy())
//                m_fluxengine.stop();
            m_fluxengine.setAddress("write");
            m_fluxengine.start();
            break;
        case 2:
            {
                QString m_address = m_fluxengine.getAddress();
                foreach (QString x, formats)
                {
                    qInfo() << Q_FUNC_INFO;
                    qInfo() << m_address + " " + x + " -C";
//                    if (m_fluxengine.busy())
//                         m_fluxengine.stop();
                    m_fluxengine.setAddress(m_address + " " + x + " -C");
                    m_fluxengine.start();
                }

            break;
            }
        default: //
            break;
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
           /*syntax: fluxengine read <profile> [<options>...]
            Use --help for option help.
            Available profiles include:
              acornadfs
              acorndfs
              aeslanier
              amiga
              ampro
              apple2
              atarist
              brother
              commodore1541
              commodore1581
              eco1
              f85
              fb100
              ibm
              macintosh
              micropolis
              mx
              tids990
              victor9k
              zilogmcz
            Or use a text file containing your own configuration.
            */
            qInfo() << Q_FUNC_INFO;

            //get the readformats and store in section
            int i = data.indexOf(":",0);
            int j = data.indexOf(":", i+1);
            for (int x=0; x < data.count(); x++)
            {
                i = data.indexOf("\n", j);
                j = data.indexOf("\n", i+1);
                if (j == -1) break;
                qInfo() << j;
                qInfo() << data.mid(i, j+1 - i).trimmed();
                Formats.append(data.mid(i, j+1 - i).trimmed());
                //i = j+1;

            }
            return Formats;
    //        foreach (QString x, Formats) {
    //            qInfo() << x;
    //        }
            //get the config for each readformat and store in section
            /*
             * input {
                  flux {
                    drive {
                    }
                  }
                }
                output {
                  image {
                    filename: "acornadfs.img"
                    img {
                    }
                  }
                }
                decoder {
                  ibm {
                    sector_id_base: 0
                  }
                }
                cylinders {
                  start: 0
                  end: 79
                }
                heads {
                  start: 0
                  end: 1
                }
                */
            foreach (QString x, Formats) {
                qInfo() << x;
            }
        }
       return Formats;

}

QStringList DialogPreferences::getConfig(QString data)
{
    QStringList Configs;
    struct FormatsDescription
    {
        QString strType;
        QString strDescription;
        QString strFilter;
        QString strDefaultfilenaam;
        QString trackstart;
        QString trackstop;
        QString headstart;
        QString headstop;
    };

    if (data.contains("encoder") || (data.contains("decoder")))
    {
        int i = data.indexOf(":",0);
        int j;
//        for (int x=0; x < data.count(); x++)
//        {
//            i = data.indexOf("\n", );
            j = data.indexOf("\n", i+1);
//            if (j == -1) break;
            qInfo() << j;
            qInfo() << data.mid(i, j+1 - i).trimmed();
            Configs.append(data.mid(i, j+1 - i).trimmed());
            //i = j+1;

//        }
        return Configs;
//        foreach (QString x, Configs) {
//            qInfo() << x;
//        }

    }
    //get the config for each readformat and store in section
    /*
     * input {
          flux {
            drive {
            }
          }
        }
        output {
          image {
            filename: "acornadfs.img"
            img {
            }
          }
        }
        decoder {
          ibm {
            sector_id_base: 0
          }
        }
        cylinders {
          start: 0
          end: 79
        }
        heads {
          start: 0
          end: 1
        }
        */
    //stringlist contains filename, start cylinder, stop cylinder, start Head, stop head.
    return Configs;
}

void DialogPreferences::output(QString data)
{
    if (data != "")
    {
        if (m_fluxengine.getAddress().contains("read") || m_fluxengine.getAddress().contains("write"))
        {
            formats = initializeformats(data);
            //save formats somewhere
        }
        if (m_fluxengine.getAddress().contains("-C"))
        {
            qInfo() << Q_FUNC_INFO;
            formats = getConfig(data);
        }

    //    foreach (QString x, formats) {
    //        qInfo() << Q_FUNC_INFO;
    //        QString m_address = m_fluxengine.getAddress();

    //        m_address = m_address + x + " -C";
    //        qInfo() << m_address;
    //        m_fluxengine.setAddress(m_address);
    //        m_fluxengine.start();
    //    }

        State = State + 1; //Data from fluxengine is processed.
//        m_fluxengine.stop();
        initializefluxengine();
   }
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
