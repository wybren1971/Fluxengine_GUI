#include "wizard.h"
#include <QtWidgets>
/*
 * This Wizard creates a string for input for fluxengine
 * output looks for example like:
 * ' read ibm -s :d=0:s=0:t=0-39 --overwrite -f ARK.flux -o ARK.imd'
 * double step is needed when one has a 80 track drive and wants to read or write 40 track disks.
 *
*/
int intSelectedDrive = 0;

QString _strOutputfile;
QString _strInputfile;
QString _strFluxFile;
QString _strInputFluxFile;
QString _strCSVfile;

const int ReadFormatDefault = 13; //IBM
const int WriteFormatDefault = 15; //IBM 1440

const int filename = 0;
const int trackstart = 1;
const int trackstop = 2;
const int headstart = 3;
const int headstop = 4;
const int description = 5;
const int filter = 6;
const int type = 7;

QVector<QVector<QString>> my_readformat;
QVector<QVector<QString>> my_writeformat;

QVector<QVector<QString>> CreateMatrix(int sizeX, int sizeY)
{
   QVector<QVector<QString>> result;
   for (int idx1 = 0; idx1 < sizeX; idx1++)
   {
      result.append(QVector<QString>());
      for (int idx2 = 0; idx2 < sizeY; idx2++)
      {
         result[idx1].append(QString());
      }
   }
   return result;
}

wizard::wizard(QWidget *parent, int intDrive)
     : QWizard(parent)
{
  setPage(Page_Intro, new IntroPage);
  setPage(Page_Read, new ReadPage());
  setPage(Page_Write, new WritePage);
  setPage(Page_Advanced, new AdvancedPage);
  setPage(Page_Conclusion, new ConclusionPage);
  intSelectedDrive = intDrive;
  setStartId(Page_Intro);
  setOption(HaveHelpButton, true);
  setPixmap(QWizard::LogoPixmap, QPixmap("sky-cow.png"));

  connect(this, &QWizard::helpRequested, this, &wizard::showHelp);

  setWindowTitle(tr("Fluxengine Wizard"));
}

void wizard::showHelp()
{
    static QString lastHelpMessage;

    QString message;

    switch (currentId()) {
    case Page_Intro:
        message = tr("This wizard will help you create the appropriate command for fluxengine "
                     "just choose what you want to do.");
        break;
    case Page_Read:
        message = tr("Choose the appropriate file format to read (most of the time IBM)"
                     "if you don't want to read all of the tracks or all of the sides (heads) you can specify which tracks and sides (heads) to read"
                     " and last but not least give a filenaam to store the information from the disk in."
                     " There is also the option to rerun the decode from the flux file, tweaking the parameters. In that case specify the 'Use Flux File'");
        break;
    case Page_Write:
        message = tr("Choose the appropriate file format to write (most of the time IBM)");
        break;
    case Page_Conclusion:
        message = tr("All the settings have been set. If you are sure push finish and fluxengine will start.");
        break;
    default:
        message = tr("This help is likely not to be of any help.");
    }

    if (lastHelpMessage == message)
        message = tr("Sorry, I already gave what help I could. "
                     "Maybe you should try the github page?");

    QMessageBox::information(this, tr("Fluxengine Wizard Help"), message);

    lastHelpMessage = message;
}
IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap("sky-cow.png"));

    topLabel = new QLabel(tr("This wizard will help you create the appropriate command for "
                             "<i>Fluxengine</i>"));
    topLabel->setWordWrap(true);

    readRadioButton = new QRadioButton(tr("&Read a disk"));
    writeRadioButton = new QRadioButton(tr("&Write a disk"));
    readRadioButton->setChecked(true);

    registerField("IntroPage.read", readRadioButton);
    registerField("IntroPage.write", writeRadioButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(readRadioButton);
    layout->addWidget(writeRadioButton);
    setLayout(layout);
}

int IntroPage::nextId() const
{
    if (writeRadioButton->isChecked()) {
        return wizard::Page_Write;
    } else {
        return wizard::Page_Read;
    }
}

ReadPage::ReadPage(QWidget *parent)
    : QWizardPage(parent)
{
    QValidator *validator = new QIntValidator(10, 99, this);
//    QRegularExpression rx("^[0-1][-][1]$");
    QRegularExpression rx("[0-1]");
    QValidator *validatorhead = new QRegularExpressionValidator(rx, this);
//    QValidator *validatorhead = new QIntValidator(0, 1, this);
    setTitle(tr("Set the read options for <i>Fluxengine</i>;"));
    setSubTitle(tr("Please choose the format of the disk to be read "
                   "and the name of the output file (e.g., wordperfect5.img)."));


    nameLabel = new QLabel(tr("F&ormat:"));
    readFormatbox = new QComboBox();

    QObject::connect(readFormatbox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatedirectorybox(int)));

    nameLabel->setBuddy(readFormatbox);


    trackLabelStart = new QLabel(tr("Tracks:"));
    trackLineEditStart = new QLineEdit;
    QObject::connect(trackLineEditStart, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(trackLineEditStart, SIGNAL(textChanged(QString)), this, SLOT(editLineBox(QString)));
    trackLineEditStart->setValidator(validator);
    trackLabelStop = new QLabel(tr(" - "));
    trackLineEditStop = new QLineEdit;
    QObject::connect(trackLineEditStop, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(trackLineEditStop, SIGNAL(textChanged(QString)), this, SLOT(editLineBox(QString)));
    trackLineEditStop->setValidator(validator);
    directoryComboBox = new QLineEdit();
    QObject::connect(directoryComboBox, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(directoryComboBox, SIGNAL(textChanged(QString)), this, SLOT(editDirectoryBox(QString)));

    HeadLineLabelStart = new QLabel(tr("Head(s):"));
    HeadLineEditStart = new QLineEdit;
    QObject::connect(HeadLineEditStart, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(HeadLineEditStart, SIGNAL(textChanged(QString)), this, SLOT(editLineBox(QString)));
    HeadLineEditStart->setValidator(validatorhead);
    headLabelStop = new QLabel(tr(" - "));
    HeadLineEditStop = new QLineEdit;
    QObject::connect(HeadLineEditStop, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(HeadLineEditStop, SIGNAL(textChanged(QString)), this, SLOT(editLineBox(QString)));
    HeadLineEditStop->setValidator(validatorhead);

    label = new QLabel("Outputfile");
    button = new QPushButton("Browse...");
    connect(button, SIGNAL(clicked()), SLOT(browse()));
    label1 = new QLabel("Write Flux File");
    button1 = new QPushButton("Browse...");
    fluxComboBox = new QLineEdit();
    QObject::connect(fluxComboBox, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(fluxComboBox, SIGNAL(textChanged(QString)), this, SLOT(editFluxBox(QString)));

    connect(button1, SIGNAL(clicked()), SLOT(browseflux()));

    label2 = new QLabel("Use Flux File");
    button2 = new QPushButton("Browse...");
    flux1ComboBox = new QLineEdit();
    QObject::connect(flux1ComboBox, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(flux1ComboBox, SIGNAL(textChanged(QString)), this, SLOT(editFlux1Box(QString)));
    connect(button2, SIGNAL(clicked()), SLOT(browsereadflux()));

    label3 = new QLabel("CSV file");
    button3 = new QPushButton("Browse...");
    CSVComboBox = new QLineEdit();
    QObject::connect(CSVComboBox, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(CSVComboBox, SIGNAL(textChanged(QString)), this, SLOT(editCSVComboBox(QString)));
    connect(button3, SIGNAL(clicked()), SLOT(browseCSVComboBox()));

    lblAdvancedSettings = new QLabel("Advanced settings");
    Checkbox = new QCheckBox;
    Checkbox->setToolTip("check this to set advanced read settings");
    lblAdvancedSettings->setToolTip("check this to set advanced read settings");
    registerField("ReadPage.format", readFormatbox);
    registerField("ReadPage.TrackStart*", trackLineEditStart);
    registerField("ReadPage.TrackStop*", trackLineEditStop);
    registerField("ReadPage.SaveOutput*", directoryComboBox);
    registerField("ReadPage.Saveflux", fluxComboBox);
    registerField("ReadPage.Save1flux", flux1ComboBox);
    registerField("ReadPage.Headstart*", HeadLineEditStart);
    registerField("ReadPage.Headstop*", HeadLineEditStop);
    registerField("ReadPage.Advanced", Checkbox);
    registerField("ReadPage.SaveCSV", CSVComboBox);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(readFormatbox, 0, 1);
    layout->addWidget(label2,1,0);
    layout->addWidget(flux1ComboBox, 1,1);
    layout->addWidget(button2, 1, 3);
    layout->addWidget(trackLabelStart, 2, 0);
    layout->addWidget(trackLineEditStart, 2, 1);
    layout->addWidget(trackLabelStop, 2, 2);
    layout->addWidget(trackLineEditStop, 2, 3);
    layout->addWidget(HeadLineLabelStart, 3, 0);
    layout->addWidget(HeadLineEditStart, 3, 1);
    layout->addWidget(headLabelStop, 3, 2);
    layout->addWidget(HeadLineEditStop, 3, 3);
    layout->addWidget(label,4,0);
    layout->addWidget(directoryComboBox, 4,1);
    layout->addWidget(button, 4, 3);
    layout->addWidget(label1,5,0);
    layout->addWidget(fluxComboBox, 5,1);
    layout->addWidget(button1, 5, 3);
    layout->addWidget(label3,6,0);
    layout->addWidget(CSVComboBox, 6,1);
    layout->addWidget(button3, 6, 3);
    layout->addWidget(lblAdvancedSettings, 7,0);
    layout->addWidget(Checkbox, 7, 1);
    layout->setColumnStretch(1,1);
    setLayout(layout);
}

void ReadPage::initializePage()
{
    QStringList readformat;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    int i =0;
    qInfo() << Q_FUNC_INFO;
    settings.beginGroup("readformats");
    while (settings.value(QString::number(i)) != "")
    {
        if (settings.value(QString::number(i)).toString() == "")
        {
            break;
        } else
        {
            readformat.append(settings.value(QString::number(i)).toString());

            qInfo() << "Readformat " << settings.value(QString::number(i)).toString();
            qInfo() << "i: " << i;
            i++;
        }
    }
    settings.endGroup();
\
    my_readformat = CreateMatrix(readformat.size(),8);
    qInfo() << "size " << my_readformat.size();
    for (i=0;i<readformat.size();i++)
    {
       QString x = readformat[i];
        settings.beginGroup("readconfigs-" + x);
        my_readformat[i][filename] =settings.value("0").toString();
        my_readformat[i][trackstart] =settings.value("1").toString();
        my_readformat[i][trackstop] =settings.value("2").toString();
        my_readformat[i][headstart] =settings.value("3").toString();
        my_readformat[i][headstop] =settings.value("4").toString();

        int last_dot = x.indexOf(".");
        QString strfilter = x.right(last_dot);
        my_readformat[i][filter] ="*." + strfilter + ", *.flux";
        my_readformat[i][type] = x;
        settings.endGroup();

        settings.beginGroup("readformatsdescription");
        my_readformat[i][description] = settings.value(QString::number(i)).toString();
        settings.endGroup();
    }
     for (i=0;i<my_readformat.size();i++)
    {
        readFormatbox->addItem(my_readformat[i][description]);
    }
    readFormatbox->setCurrentIndex(ReadFormatDefault);                                                         //set IBM as standard

}

int ReadPage::nextId() const
{
    _strOutputfile = (directoryComboBox->text());
    _strFluxFile = fluxComboBox->text();
    _strInputFluxFile = flux1ComboBox->text();
    _strCSVfile = CSVComboBox->text();

    if (Checkbox->isChecked())
    {
        return wizard::Page_Advanced;
    } else
    {
        return wizard::Page_Conclusion;
    }
}

void ReadPage::editLineBox(QString dir)
{
        QList<QWidget*> mylineEdits = this->findChildren<QWidget*>();
        QListIterator<QWidget*> it(mylineEdits); // iterate through the list of widgets
        QWidget *lineEditField;
        while (it.hasNext()) {
            lineEditField = it.next(); // take each widget in the list
            if(QLineEdit *lineE = qobject_cast<QLineEdit*>(lineEditField)) {  // check if iterated widget is of type QLineEdit
                //
                if (lineE->hasFocus())
                  {
                    if (dir == "")
                    {
                        lineE->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
                        emit completeChanged();
                    } else
                    {
                        lineE->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
                        emit completeChanged();
                    }
                  }
             }
        }
}

void ReadPage::editDirectoryBox(QString dir)
{
    if (dir != "")
    {
        //haal het einde van de string af. De filenaam:-)
        int last_dot = dir.lastIndexOf("/");
        QString dir1 = dir.left(last_dot);
        const QFileInfo outputDir(dir1);
        if (!outputDir.isDir()) {
            directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
            emit completeChanged();
        } else
        {
            int last_dot = dir.lastIndexOf(".");
            int size = dir.size();
            if ((last_dot == -1) || ((size - last_dot) > 5))
              //check for valid filenaam. ends with .xxxx
            {
                directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
                emit completeChanged();
            } else
            {
                directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
                emit completeChanged();
            }
        }
    } else
    {
        directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
        emit completeChanged();
    }
}
void ReadPage::editFluxBox(QString dir)
{
    if (dir != "")
    {
        //haal het einde van de string af. De filenaam:-)
        int last_dot = dir.lastIndexOf("/");
        QString dir1 = dir.left(last_dot);
        const QFileInfo outputDir(dir1);
        if (!outputDir.isDir()) {
            fluxComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
            emit completeChanged();
        } else
        {
            int last_dot = dir.lastIndexOf(".");
            int size = dir.size();
            if ((last_dot == -1) || ((size - last_dot) > 5))
              //check for valid filenaam. ends with .xxx
            {
                fluxComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
                emit completeChanged();
            } else
            {
                fluxComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
                emit completeChanged();
            }
        }
    } else
    {
        fluxComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
        emit completeChanged();
    }
}
void ReadPage::editFlux1Box(QString dir)
{
    //bepaal welke linedit the focus heeft
    if (dir != "")
    {
        //haal het einde van de string af. De filenaam:-)
        int last_dot = dir.lastIndexOf("/");
        QString dir1 = dir.left(last_dot);
        const QFileInfo outputDir(dir1);
        if (!outputDir.isDir()) {
            flux1ComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
            emit completeChanged();
        } else
        {
            int last_dot = dir.lastIndexOf(".");
            int size = dir.size();
            if ((last_dot == -1) || ((size - last_dot) > 5))
              //check for valid filenaam. ends with .xxx
            {
                flux1ComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
                emit completeChanged();
            } else
            {
                flux1ComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
                emit completeChanged();
            }
        }
    } else
    {
        flux1ComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
        emit completeChanged();
    }
}

void ReadPage::editCSVComboBox(QString dir)
{
    //bepaal welke linedit the focus heeft
    if (dir != "")
    {
        //haal het einde van de string af. De filenaam:-)
        int last_dot = dir.lastIndexOf("/");
        QString dir1 = dir.left(last_dot);
        const QFileInfo outputDir(dir1);
        if (!outputDir.isDir()) {
            CSVComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
            emit completeChanged();
        } else
        {
            int last_dot = dir.lastIndexOf(".");
            int size = dir.size();
            if ((last_dot == -1) || ((size - last_dot) > 5))
              //check for valid filenaam. ends with .xxx
            {
                CSVComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
                emit completeChanged();
            } else
            {
                CSVComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
                emit completeChanged();
            }
        }
    } else
    {
        CSVComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
        emit completeChanged();
    }
}

bool ReadPage::isComplete() const
{
    bool Complete = true;
    QList<QWidget*> mylineEdits = this->findChildren<QWidget*>();
    QListIterator<QWidget*> it(mylineEdits); // iterate through the list of widgets
    QWidget *lineEditField;
    while (it.hasNext())
    {
        lineEditField = it.next(); // take each widget in the list
        if(QLineEdit *lineE = qobject_cast<QLineEdit*>(lineEditField))
         {  // check if iterated widget is of type QLineEdit
            if (lineE->styleSheet() == "QLineEdit { background: rgb(255,0,0); }" )
            {

                return false;
            }
            else
            {
                Complete = true;
            }

        } else
        {
            Complete = true;
        }
        //qInfo() << "lineeditwidget";
    }
    return Complete;
}

void ReadPage::updatedirectorybox(int index)
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    qInfo() << Q_FUNC_INFO;
    qInfo() << "index: " << index;
    if (index >= 0)
    {
        trackLineEditStart->setFocus();
        trackLineEditStart->setText(my_readformat[index][trackstart]);
        qInfo() << "index: " << my_readformat[index][trackstart];

        QString drivetext = "drive" + QString::number(intSelectedDrive)+ "40track";
        if (settings.value(drivetext).toBool())
            //40track drive
        {
            trackLineEditStop->setFocus();
            trackLineEditStop->setText("39");
        } else
        {
            trackLineEditStop->setFocus();
            trackLineEditStop->setText(my_readformat[index][trackstop]);
        }
        HeadLineEditStart->setFocus();
        HeadLineEditStart->setText(my_readformat[index][headstart]);
        if (my_readformat[index][headstop] == "")
        {
            //empty no second head
//            QRegularExpression rx("[0]");
//            QValidator *validatorhead = new QRegularExpressionValidator(rx, this);
//            HeadLineEditStart->setValidator(validatorhead);
            HeadLineEditStop->setVisible(false);
            headLabelStop->setVisible(false);
            HeadLineEditStop->setText(my_readformat[index][headstop]);
            HeadLineEditStart->setDisabled(true);

        } else
        {
            HeadLineEditStop->setVisible(true);
            headLabelStop->setVisible(true);
            HeadLineEditStop->setFocus();
            HeadLineEditStop->setText(my_readformat[index][headstop]);
            HeadLineEditStart->setDisabled(false);
        }

        strFilter = my_readformat[index][filter];

        strFile =  "/" + my_readformat[index][filename];
        QString directory = settings.value("datalocation").toString();
        directoryComboBox->clear();
        if (directory != "")
        {
            directoryComboBox->setText(directory + strFile);

        } else
        {
            directoryComboBox->setText(QDir::currentPath() + strFile);
        }
        directoryComboBox->setFocus();
    }
}

void ReadPage::browse()
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFilter = my_readformat[readFormatbox->currentIndex()][filter];
    strFile =  "/" + my_readformat[readFormatbox->currentIndex()][filename];
    QString directory = settings.value("datalocation").toString();
    if (directory == "")
    {
        directory = QFileDialog::getSaveFileName(this,
                            tr("Find Files"), QDir::currentPath()+strFile,strFilter);
    } else
    {
        directory = QFileDialog::getSaveFileName(this,
                            tr("Find Files"), directory + strFile,strFilter);
    }
    if (!directory.isEmpty()) {
        directoryComboBox->setText(directory);
    }
}

void ReadPage::browseflux()
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFile = my_readformat[readFormatbox->currentIndex()][filename];
    QString desired =  "/" + strFile.mid(0,strFile.indexOf(".")) + ".flux";
    strFilter = my_readformat[readFormatbox->currentIndex()][filter];
    QString directory = settings.value("fluxlocation").toString();
    if (directory == "")
    {
        directory = QFileDialog::getSaveFileName(this,
                            tr("Find Files"), QDir::currentPath() + desired,strFilter);
    } else
    {
        directory = QFileDialog::getSaveFileName(this,
                            tr("Find Files"), directory + desired,strFilter);
    }

    if (!directory.isEmpty()) {
        fluxComboBox->setText(directory);
    }
}

void ReadPage::browsereadflux()
{
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFile = my_readformat[readFormatbox->currentIndex()][filename];
    QString directory = settings.value("fluxlocation").toString();
    if (directory == "")
    {
        directory = QFileDialog::getOpenFileName(this,
                            tr("Find Files"), QDir::currentPath(), "*.flux");
    } else
    {
        directory = QFileDialog::getOpenFileName(this,
                            tr("Find Files"), directory, "*.flux");

    }

    if (!directory.isEmpty()) {
        flux1ComboBox->setText(directory);
    }
}

void ReadPage::browseCSVComboBox()
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFile = my_readformat[readFormatbox->currentIndex()][filename];
    QString desired =  "/" + strFile.mid(0,strFile.indexOf(".")) + ".csv";
    strFilter = "*.csv";
    QString directory = settings.value("csvlocation").toString();
    if (directory == "")
    {
        directory = QFileDialog::getSaveFileName(this,
                            tr("Find Files"), QDir::currentPath() + desired,strFilter);
    } else
    {
        directory = QFileDialog::getSaveFileName(this,
                            tr("Find Files"), directory + desired,strFilter);
    }

    if (!directory.isEmpty()) {
        CSVComboBox->setText(directory);
    }
}

WritePage::WritePage(QWidget *parent)
    : QWizardPage(parent)
{
    QValidator *validator = new QIntValidator(10, 99, this);
    QRegularExpression rx("^[0-1][-][1]$");
    QValidator *validatorhead = new QRegularExpressionValidator(rx, this);
    setTitle(tr("Write a Disk with <i>Fluxengine</i>;"));
    setSubTitle(tr("Choose the type of disk you want to write and the name of the file you want to write to disk."));

    nameLabel = new QLabel(tr("F&ormat:"));
    writeFormatbox = new QComboBox();

    connect(writeFormatbox, SIGNAL(currentIndexChanged(int)), SLOT(Update(int)));
    nameLabel->setBuddy(writeFormatbox);

    trackLabelStart = new QLabel(tr("Tracks:"));
    trackLineEditStart = new QLineEdit;
    QObject::connect(trackLineEditStart, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(trackLineEditStart, SIGNAL(textChanged(QString)), this, SLOT(editLineBox(QString)));
    trackLineEditStart->setText("0");
    trackLineEditStart->setValidator(validator);
    trackLabelStop = new QLabel(tr(" - "));
    trackLineEditStop = new QLineEdit;
    QObject::connect(trackLineEditStop, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(trackLineEditStop, SIGNAL(textChanged(QString)), this, SLOT(editLineBox(QString)));
    trackLineEditStop->setText("79");
    trackLineEditStop->setValidator(validator);
    directoryComboBox = new QLineEdit();
    QObject::connect(directoryComboBox, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(directoryComboBox, SIGNAL(textChanged(QString)), this, SLOT(editDirectoryBox(QString)));

    // -s :d=0:s=0:t=0-39
    HeadLabelStart = new QLabel(tr("Head(s):"));
    HeadLineEditStart = new QLineEdit;
    QObject::connect(HeadLineEditStart, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(HeadLineEditStart, SIGNAL(textChanged(QString)), this, SLOT(editLineBox(QString)));
    HeadLineEditStart->setValidator(validatorhead);
    headLabelStop = new QLabel(tr(" - "));
    HeadLineEditStop = new QLineEdit;
    QObject::connect(HeadLineEditStop, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));
    QObject::connect(HeadLineEditStop, SIGNAL(textChanged(QString)), this, SLOT(editLineBox(QString)));
    HeadLineEditStop->setValidator(validatorhead);


    label = new QLabel("Inputfile");
    button = new QPushButton("Browse...");
    connect(button, SIGNAL(clicked()), SLOT(browse()));
    registerField("WritePage.format", writeFormatbox);
    registerField("WritePage.TrackStart", trackLineEditStart);
    registerField("WritePage.TrackStop", trackLineEditStop);
    registerField("WritePage.OpenInput*", directoryComboBox);
    registerField("WritePage.Headstart", HeadLineEditStart);
    registerField("WritePage.Headstop", HeadLineEditStop);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(writeFormatbox, 0, 1);
    layout->addWidget(trackLabelStart, 1, 0);
    layout->addWidget(trackLineEditStart, 1, 1);
    layout->addWidget(trackLabelStop, 1, 2);
    layout->addWidget(trackLineEditStop, 1, 3);
    layout->addWidget(HeadLabelStart, 2, 0);
    layout->addWidget(HeadLineEditStart, 2, 1);
    layout->addWidget(headLabelStop, 2, 2);
    layout->addWidget(HeadLineEditStop, 2, 3);
    layout->addWidget(label,3,0);
    layout->addWidget(directoryComboBox, 3,1);
    layout->addWidget(button, 3, 3);
    setLayout(layout);
}
bool WritePage::isComplete() const
{
    bool Complete = true;
    QList<QWidget*> mylineEdits = this->findChildren<QWidget*>();
    QListIterator<QWidget*> it(mylineEdits); // iterate through the list of widgets
    QWidget *lineEditField;
    while (it.hasNext())
    {
        lineEditField = it.next(); // take each widget in the list
        if(QLineEdit *lineE = qobject_cast<QLineEdit*>(lineEditField))
        {  // check if iterated widget is of type QLineEdit
            if (lineE->styleSheet() == "QLineEdit { background: rgb(255,0,0); }" )
            {

                return false;
            }
            else
            {
                if (lineE->text() != "")
                {
                    Complete = true;
                } else
                {
                    Complete = false;
                }
            }

        } else
        {
            Complete = true;
        }
        //qInfo() << Complete;
    }
    return Complete;
}

void WritePage::initializePage()
{
    QStringList writeformat;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    int i =0;
    qInfo() << Q_FUNC_INFO;
    settings.beginGroup("writeformats");
    while (settings.value(QString::number(i)) != "")
    {
        if (settings.value(QString::number(i)).toString() == "")
        {
            break;
        } else
        {
            writeformat.append(settings.value(QString::number(i)).toString());

            qInfo() << "Writeformat " << settings.value(QString::number(i)).toString();
            qInfo() << "i: " << i;
            i++;
        }
    }
    settings.endGroup();
\
    my_writeformat = CreateMatrix(writeformat.size(),8);
    qInfo() << "size " << my_writeformat.size();
    for (i=0;i<writeformat.size();i++)
    {
       QString x = writeformat[i];
        settings.beginGroup("writeconfigs-" + x);
        my_writeformat[i][filename] =settings.value("0").toString();
        my_writeformat[i][trackstart] =settings.value("1").toString();
        my_writeformat[i][trackstop] =settings.value("2").toString();
        my_writeformat[i][headstart] =settings.value("3").toString();
        my_writeformat[i][headstop] =settings.value("4").toString();

        int last_dot = my_writeformat[i][filename].indexOf(".");
        qInfo() << last_dot;
        QString strfilter = my_writeformat[i][filename].right(my_writeformat[i][filename].size() - last_dot);
        my_writeformat[i][filter] ="*" + strfilter;
        my_writeformat[i][type] = x;
        settings.endGroup();

        settings.beginGroup("writeformatsdescription");
        my_writeformat[i][description] = settings.value(QString::number(i)).toString();
        settings.endGroup();
    }
     for (i=0;i<my_writeformat.size();i++)
    {
        writeFormatbox->addItem(my_writeformat[i][description]);
    }

    writeFormatbox->setCurrentIndex(WriteFormatDefault);   //set IBM as standard
}

int WritePage::nextId() const
{
    return wizard::Page_Conclusion;

}

void WritePage::editDirectoryBox(QString dir)
{
    { //Check if the file to write exists
        if (dir != "")
        {
            const QFileInfo outputDir(dir);
            if (!outputDir.isFile()) {
                directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
                emit completeChanged();
            } else
            {
                directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
                _strInputfile = dir;
                emit completeChanged();
            }
        } else
        {
            directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
            emit completeChanged();
        }
    }
}

void WritePage::editLineBox(QString dir)
{
        QList<QWidget*> mylineEdits = this->findChildren<QWidget*>();
        QListIterator<QWidget*> it(mylineEdits); // iterate through the list of widgets
        QWidget *lineEditField;
        while (it.hasNext()) {
            lineEditField = it.next(); // take each widget in the list
            if(QLineEdit *lineE = qobject_cast<QLineEdit*>(lineEditField)) {  // check if iterated widget is of type QLineEdit
                //
                if (lineE->hasFocus())
                  {
                    if (dir == "")
                    {
                        lineE->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
                        emit completeChanged();
                    } else
                    {
                        lineE->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
                        emit completeChanged();
                    }
                  }
             }
        }
}


void WritePage::browse()
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFilter = my_writeformat[writeFormatbox->currentIndex()][filter];
    strFile = "/" + my_writeformat[writeFormatbox->currentIndex()][filename];

    QString directory = settings.value("datalocation").toString();
    if (directory == "")
    {
        directory = QFileDialog::getOpenFileName(this,
                            tr("Find Files"), QDir::currentPath() + strFile,strFilter);
    } else
    {
        directory = QFileDialog::getOpenFileName(this,
                            tr("Find Files"), directory + strFile,strFilter);

    }

    if (!directory.isEmpty()) {
        directoryComboBox->setText(directory);
        directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
        _strInputfile = directory;
    } else
    {
        directoryComboBox->setStyleSheet("QLineEdit { background: rgb(255,0,0); }");
    }
}

void WritePage::Update(int index)
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    if (index >= 0)
    {
        trackLineEditStart->setFocus();
        trackLineEditStart->setText(my_writeformat[index][trackstart]);
        trackLineEditStart->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");

        QString drivetext = "drive" + QString::number(intSelectedDrive)+ "40track";
        if (settings.value(drivetext).toBool())
            //40track drive
        {
            trackLineEditStop->setFocus();
            trackLineEditStop->setText("39");
            trackLineEditStop->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");

        } else
        {
            trackLineEditStop->setFocus();
            trackLineEditStop->setText(my_writeformat[index][trackstop]);
            trackLineEditStop->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
        }
        HeadLineEditStart->setFocus();
        HeadLineEditStart->setText(my_writeformat[index][headstart]);
//        HeadLineEditStart->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
        HeadLineEditStop->setFocus();
        if (my_writeformat[index][headstop] == "")
        {
            //empty no second head
//            QRegularExpression rx("[0]");
//            QValidator *validatorhead = new QRegularExpressionValidator(rx, this);
//            HeadLineEditStart->setValidator(validatorhead);
            HeadLineEditStop->setVisible(false);
            headLabelStop->setVisible(false);
            HeadLineEditStop->setText(my_writeformat[index][headstart]);
            HeadLineEditStart->setDisabled(true);
            trackLineEditStop->setFocus();


        } else
        {
            HeadLineEditStop->setVisible(true);
            headLabelStop->setVisible(true);
            HeadLineEditStop->setFocus();
            HeadLineEditStop->setText(my_writeformat[index][headstop]);
//            HeadLineEditStop->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
            HeadLineEditStart->setDisabled(false);
        }
        editDirectoryBox(directoryComboBox->text());
    }
}
AdvancedPage::AdvancedPage(QWidget *parent)
    : QWizardPage(parent)
{
    /*
     * Other important flags
     *
     *   These flags apply to many operations and are useful for modifying the overall behaviour.
     *
     *   --input.flux.drive.revolutions=X
     *
     *  When reading, spin the disk X times. X can be a floating point number. The default is usually 1.2. Some formats default to 1. Increasing the number will sample more data, and can be useful on dubious disks to try and get a better read.
     *
     *  --input.flux.drive.sync_with_index=true|false
     *   Wait for an index pulse before starting to read the disk. (Ignored for write operations.) By default FluxEngine doesn't, as it makes reads faster, but when diagnosing disk problems it's helpful to have all your data start at the same place each time.
     *   --decoder.pulse_debounce_threshold controls whether FluxEngine ignores pairs of pulses in rapid succession. This is common on some disks (I've observed them on Brother word processor disks).
     *
     *   --decoder.clock_interval_bias adds a constant bias to the intervals between pulses before doing decodes. This is very occasionally necessary to get clean reads --- for example, if the machine which wrote the disk always writes pulses late. If you try this, use very small numbers (e.g. 0.02). Negative values are allowed.
     */
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    revolutionsLabelexplain = new QLabel;
    retriesLabelexplain = new QLabel;
    syncLabelexplain = new QLabel;
    debounceLabelexplain = new QLabel;
    clock_intervalLabelexplain = new QLabel;

    revolutionsLabel = new QLabel;
    syncLabel = new QLabel;
    retriesLabel = new QLabel;
    debounceLabel = new QLabel;
    clock_intervalLabel = new QLabel;

    revolutionsEdit = new QLineEdit;
    syncEdit = new QCheckBox;
    retriesEdit = new QLineEdit;
    debounceLabelEdit = new QLineEdit;
    clock_intervalLabelEdit = new QLineEdit;

    revolutionsLabelexplain->setWordWrap(true);
    retriesLabelexplain->setWordWrap(true);
    syncLabelexplain->setWordWrap(true);
    debounceLabelexplain->setWordWrap(true);
    clock_intervalLabelexplain->setWordWrap(true);

    revolutionsLabelexplain->setText("When reading, spin the disk X times. X can be a floating point number. The default is usually 1.2. Some formats default to 1. Increasing the number will sample more data, and can be useful on dubious disks to try and get a better read.");
    retriesLabelexplain->setText("If the disk is particularly dodgy, you can force FluxEngine not to retry failed reads with --retries=0. This reduces head movement. This is not recommended.");
    syncLabelexplain->setText("Wait for an index pulse before starting to read the disk. (Ignored for write operations.) By default FluxEngine doesn't, as it makes reads faster, but when diagnosing disk problems it's helpful to have all your data start at the same place each time.");
    debounceLabelexplain->setText("pulse_debounce_threshold controls whether FluxEngine ignores pairs of pulses in rapid succession. This is common on some disks (I've observed them on Brother word processor disks). The value typically varies from 0.0 to 0.5; the default is 0.2.");
    clock_intervalLabelexplain->setText("clock_interval_bias adds a constant bias to the intervals between pulses before doing decodes. This is very occasionally necessary to get clean reads --- for example, if the machine which wrote the disk always writes pulses late. If you try this, use very small numbers (e.g. 0.02). Negative values are allowed.");

    QFont font = revolutionsLabelexplain->font();
    font.setPointSize(8);
    revolutionsLabelexplain->setFont(font);
    retriesLabelexplain->setFont(font);
    syncLabelexplain->setFont(font);
    debounceLabelexplain->setFont(font);
    clock_intervalLabelexplain->setFont(font);

    revolutionsLabel->setText("Revolutions:");
    syncLabel->setText("Wait for index pulse:");
    retriesLabel->setText("Number of retries:");
    debounceLabel->setText("Debounce value:");
    clock_intervalLabel->setText("constant bias:");

//    QValidator *validator = new QValidator(1, 99, this);

    //"0?\\.\\d{0,10}"
    QRegExp rx("[0-9]\\.\\d{1,2}");
    QValidator *revolutionsvalidator = new QRegExpValidator(rx, this);
    revolutionsEdit->setValidator(revolutionsvalidator);
    QIntValidator *retriesvalidator= new QIntValidator(0,20);
    retriesEdit->setValidator(retriesvalidator);
//    QDoubleValidator *debouncevalidator= new QDoubleValidator(0.00, 0.50, 2);
    QRegExp rx1("0?\\.\\d{0,2}");
    QValidator *debouncevalidator = new QRegExpValidator(rx1, this);
    debounceLabelEdit->setValidator(debouncevalidator);
//    QDoubleValidator *clock_intervalvalidator= new QDoubleValidator(-1.000, 1.000, 3);
    QRegExp rx2("-?0?\\.\\d{0,3}");
    QValidator *clock_intervalvalidator = new QRegExpValidator(rx2, this);
    clock_intervalLabelEdit->setValidator(clock_intervalvalidator);

    registerField("AdvancedPage.revolutions", revolutionsEdit);
    registerField("AdvancedPage.retries", retriesEdit);
    registerField("AdvancedPage.sync", syncEdit);
    registerField("AdvancedPage.clock_interval", clock_intervalLabelEdit);
    registerField("AdvancedPage.debounce", debounceLabelEdit);

    QGridLayout *layout = new QGridLayout;
    layout->setSizeConstraint(layout->SetFixedSize);
    layout->addWidget(retriesLabel, 0, 0);
    layout->addWidget(retriesEdit, 0, 1);
    layout->addWidget(retriesLabelexplain, 0, 2);
    layout->addWidget(revolutionsLabel, 1, 0);
    layout->addWidget(revolutionsEdit, 1, 1);
    layout->addWidget(revolutionsLabelexplain, 1, 2);
    layout->addWidget(syncLabel, 2, 0);
    layout->addWidget(syncEdit, 2, 1);
    layout->addWidget(syncLabelexplain, 2, 2);
    layout->addWidget(clock_intervalLabel, 3, 0);
    layout->addWidget(clock_intervalLabelEdit, 3, 1);
    layout->addWidget(clock_intervalLabelexplain, 3, 2);
    layout->addWidget(debounceLabel, 4, 0);
    layout->addWidget(debounceLabelEdit, 4, 1);
    layout->addWidget(debounceLabelexplain, 4, 2);
    setLayout(layout);


}

void AdvancedPage::initializePage()
{
    if (field("IntroPage.read") == "True")
    {
        setTitle(tr("Set the advanced read option to tweak fluxengine when reading the disk."));
        setSubTitle(tr("Be careful what you choose here. Leave empty to ignore"));

    } else
    {
        setTitle(tr("Set the advanced write option to tweak fluxengine when reading the disk."));
        setSubTitle(tr("Be careful what you choose here. Leave empty to ignore"));
    }

}

int AdvancedPage::nextId() const
{
    return wizard::Page_Conclusion;

}

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);

    registerField("Fluxengine.command", bottomLabel);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bottomLabel);
    setLayout(layout);
}

int ConclusionPage::nextId() const
{

    return -1;
}

void ConclusionPage::initializePage()
{
    bottomLabel->setText(getData());
    if (field("IntroPage.read") == "True")
    {
        setTitle(tr("The fluxengine wizard had gathered all the necessary settings to proceed reading the disk."));
        setSubTitle(tr("The command for fluxengine is shown below. Press finish to start reading"));

    } else
    {
        setTitle(tr("The fluxengine wizard had gathered all the necessary settings to proceed writing the disk."));
        setSubTitle(tr("The command for fluxengine is shown below. Press finish to start writing"));
    }
}

QString ConclusionPage::getData()
{
    QString command;
    QString strFormat;
    QString strDisk;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    command = "";
    strFormat = "";
    strDisk = "";

    if (field("IntroPage.read") == "True")
        //./fluxengine read commodore1541 -s drive:0 -o uridium.d64 --copy-flux-to uridium.flux
    {
       strFormat = "read ";
       int index = field("ReadPage.format").toInt();

       strDisk = my_readformat[index][type];
       strFormat.append(strDisk);
       QString TrackStart = field("ReadPage.TrackStart").toString();
       QString TrackStop = field("ReadPage.TrackStop").toString();

       if (_strInputFluxFile != "")
       {
           strFormat.append(" -s " + _strInputFluxFile);
       } else
       {
           strFormat.append(" -s drive:" + QString::number(intSelectedDrive));
       }
       strFormat.append(" -c ");
       strFormat.append(TrackStart);
       strFormat.append("-");
       QString drivetext = "drive" + QString::number(intSelectedDrive)+ "40track";
       if ((TrackStop.toInt() > 39) && (settings.value(drivetext).toBool()))
       {
           //40 track drive with 80track operand use doublestep x2
           TrackStop = TrackStop + "x2";
       }

       strFormat.append(TrackStop);

       QString Headstart = field("ReadPage.Headstart").toString();
       QString Headstop = field("ReadPage.Headstop").toString();
       if (Headstart > Headstop)
           //swap around)
       {
           QString tmp = Headstart;
           Headstart = Headstop;
           Headstop = tmp;
       }
       strFormat.append(" -h " + Headstart);
       if ((Headstop == "") || (Headstop == Headstart))
       {
           //nothing
       } else
       {
           strFormat.append("-" + Headstop);
       }

       strFormat.append(" -o ");
       strFormat.append(_strOutputfile);

//read ibm -s :d=0:s=0:t=0-39 --overwrite -f ARK.flux -o ARK.imd'
       if (_strFluxFile != "")
       {
           strFormat.append(" --copy-flux-to ");
           strFormat.append(_strFluxFile);
       }
       if (_strCSVfile != "")
       {
           strFormat.append(" --decoder.write_csv_to=");
           strFormat.append(_strCSVfile);
       }
       if (field("ReadPage.Advanced") == "true")
       {
           QString retries = field("AdvancedPage.retries").toString();
           QString revolutions = field("AdvancedPage.revolutions").toString();
           QString sync = field("AdvancedPage.sync").toString();
           QString clock_interval = field("AdvancedPage.clock_interval").toString();
           QString debounce = field("AdvancedPage.debounce").toString();
//           qInfo() << retries;
           if (retries != "")
           {
               strFormat.append(" --decoder.retries=" + retries);
           }
           if (revolutions != "")
           {
               strFormat.append(" --input.flux.drive.revolutions=" + revolutions);
           }
           if (clock_interval != "")
           {
               strFormat.append(" --decoder.clock_interval_bias " + clock_interval);
           }
           if (debounce != "")
           {
               strFormat.append(" --decoder.pulse_debounce_threshold " + debounce);
           }
           if (sync ==  "true")
           {
               strFormat.append(" --input.flux.drive.sync_with_index="+sync);
           }
       }
       command.append(strFormat);
    }
    else {
        strFormat = "write ";
        int index = field("WritePage.format").toInt();

        strDisk = my_writeformat[index][type];
        strFormat.append(strDisk);
        strFormat.append(" -i ");
        strFormat.append(_strInputfile);

        QString TrackStart = field("WritePage.TrackStart").toString();
        QString TrackStop = field("WritePage.TrackStop").toString();

        strFormat.append(" -d drive:" + QString::number(intSelectedDrive));
        strFormat.append(" -c ");
        strFormat.append(TrackStart);
        strFormat.append("-");
        QString drivetext = "drive" + QString::number(intSelectedDrive)+ "40track";
        if (((TrackStop.toInt()) > 39) && (settings.value(drivetext).toBool()))
        {
            //40 track drive with 80track operand use doublestep x2
            TrackStop = TrackStop + "x2";
        }
        strFormat.append(TrackStop);

        QString writeHeadstart = field("WritePage.Headstart").toString();
        QString writeHeadstop = field("WritePage.Headstop").toString();
        if (writeHeadstart > writeHeadstop)
            //swap around)
        {
            QString tmp = writeHeadstart;
            writeHeadstart = writeHeadstop;
            writeHeadstop = tmp;
        }
        strFormat.append(" -h " + writeHeadstart);
        if ((writeHeadstop == "") || (writeHeadstop == writeHeadstart))
        {
            //nothing
        } else
        {
            strFormat.append("-" + writeHeadstop);
        }
        command.append(strFormat);
    }

    settings.setValue("Fluxengine.command",command);
    return command;
}

QString wizard::getData()
{

    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    return settings.value("Fluxengine.command").toString();
}
