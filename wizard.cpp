#include "wizard.h"
#include <QtWidgets>
/*
 * This Wizard creates a string for input for fluxengine
 * output looks for example like:
 * ' read ibm -s :d=0:s=0:t=0-39 --overwrite -f ARK.flux -o ARK.imd'
 * double step is needed when one had a 80 track drive and wants to read or write 40 track disks.
 *
*/
int intSelectedDrive = 0;

struct FormatsDescription
{
    QString strType;
    QString strDescription;
    QString strFilter;
    QString strDefaultfilenaam;
    QString trackstart;
    QString trackstop;
    QString Heads;
};

QString _strOutputfile;
QString _strInputfile;
QString _strFluxFile;
QString _strInputFluxFile;

const int readformats = 20;
const int writeformats = 18;

const int ReadFormatDefault = 13; //IBM
const int WriteFormatDefault = 14; //IBM 1440

/*
 *
 *   acornadfs
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

 *
 */


/*
 * For now i use structs but it is better to read the proto tekst definitions.
 * this is for a next release
 *
 *          Floppy disk physical characteristics (https://en.wikipedia.org/wiki/List_of_floppy_disk_formats)
(capacity and tracks are nominal, per side)
Size                                    Density 	Tracks      tpi 	bpi 	Coercivity 	Unformatted capacity per side
21⁄2-inch[16][17]                       Single       16[16][17]  48[16]                              64 KB[16][17]
31⁄2-inch                               Double[18] 	40[18]      67.5[18]8650[18] 	600 Oe          250 KB
                                                    80          135 	8717        600-665 Oe      500 KB
                                        High        80          135 	17434       720-750 Oe      1000 KB
                                        Extended 	80          135 	34868       900 Oe          2000 KB
                                        Triple[12] 	240[11] 	406.5[11] 	36700[11]               6500 KB
51⁄4-inch                               Single/Double 40        48 	5876 	300 Oe                  250 KB
                                        Double      80          62.5                                (Apple FileWare)
                                        Quad        77          100 		300 Oe                  500 KB (Micropolis-compatible)
                                        Quad        80          96      5922300 Oe                  500 KB
                                        High        80          96      9646 	600 Oe              833 KB
8-inch                              Single/Double 	77          48              300 Oe              1000 KB
 */

FormatsDescription my_readformats[readformats] = {
    { "acornadfs", "Reads Acorn ADFS disks", "*.img *.flux", "acornadfs.img", "0", "79", "0-1"},
    { "acorndfs", "Reads Acorn DFS disks", "*.img *.flux", "acorndfs.img", "0", "79", "0"},
    { "aeslanier", "Reads AES Lanier disks", "*.img *.flux", "aeslanier.img", "0", "76", "0"},
    { "amiga", "Reads Commodore Amiga disks", "*.adf *.flux", "amiga.adf", "0", "79", "0-1"},
    { "ampro", "Reads Ampro disks", "*.img *.flux", "ampro.img", "0", "79", "0"},
    { "apple2", "Reads Apple II disks", "*.img *.flux", "apple2.img", "0", "79", "0"},
    { "atarist", "Reads Atari ST disks", "*.img *.flux *.st", "atarist.st", "0", "81", "0-1"},
    { "brother", "Reads 120kB and 240kB Brother word processor disks", "*.img *.flux", "brother.img", "0", "81", "0"},
    { "commodore1541", "Reads Commodore 64 disks in 1541 format", "*.d64 *.flux", "commodore1541.d64", "0", "39", "0"},
    { "commodore1581", "Reads Commodore 64 disks in 1581 format", "*.d81 *.flux", "commodore1581.d81", "0", "79", "0-1"},
    { "eco1", "Reads eco1 disks", "*.img *.flux", "eco1.img", "0", "81", "0-1"},
    { "f85", "Reads Durango F85 disks", "*.img *.flux", "f85.img", "0", "79", "0"},
    { "fb100", "Reads FB100 disks", "*.img *.flux", "fb100.img", "0", "39", "0"},
    { "ibm", "Reads the ubiquitous IBM format disks (Most common)", "*.img *.imd *.flux", "ibm.img", "0", "81", "0-1"},
    { "macintosh", "Reads Apple Macintosh disks", "*.diskcopy *.flux", "macintosh.diskcopy", "0", "79", "0-1"},
    { "micropolis", "Reads Micropolis disks", "*.img *.flux", "micropolis.img", "0", "76", "0-1"},
    { "mx", "Reads MX disks", "*.img *.flux", "mx.img", "0", "79", "0-1"},
    { "tids990", "Reads Texas Instruments DS990 disks", "*.img *.flux", "tids990.img", "0", "76", "0-1"},
    { "victor9k", "Reads Victor 9000 disks", "*.img *.flux", "victor9k.img", "0", "79", "0"},
    { "zilogmcz", "Reads Zilog MCZ disks", "*.img *.flux", "zilogmcz.img", "0", "76", "0"}
};


/*
 * Available profiles include:
  amiga
  atarist360
  atarist370
  atarist400
  atarist410
  atarist720
  atarist740
  atarist800
  atarist820
  brother120
  brother240
  commodore1541
  commodore1581
  hplif770
  ibm1440
  ibm720
  macintosh
  tids990

 *
 *
 *
 */
FormatsDescription my_writeformats[writeformats] = {
    { "amiga", "Writes Commodore Amiga disks", "*.adf", "amiga.adf", "0", "79", "0-1"},
    { "atarist360", "Writes 360 kB Atari st disks", "*.img", "atarist360.st", "0", "79", "0-1"},
    { "atarist370", "Writes 370 kB Atari st disks", "*.img", "atarist370.st", "0", "81", "0"},
    { "atarist400", "Writes 400 kB Atari st disks", "*.img", "atarist400.st", "0", "79", "0"},
    { "atarist410", "Writes 410 kB Atari st disks", "*.img", "atarist410.st", "0", "81", "0"},
    { "atarist720", "Writes 720 kB Atari st disks", "*.img", "atarist720.st", "0", "79", "0-1"},
    { "atarist740", "Writes 740 kB Atari st disks", "*.img", "atarist740.st", "0", "81", "0-1"},
    { "atarist800", "Writes 800 kB Atari st disks", "*.img", "atarist800.st", "0", "79", "0-1"},
    { "atarist820", "Writes 820 kB Atari st disks", "*.img", "atarist820.st", "0", "81", "0-1"},
    { "brother120", "Writes 120 kB Brother word processor disks", "*.img", "brother120.img", "0", "39", "0"},
    { "brother240", "Writes 240 kB Brother word processor disks", "*.img", "brother240.img", "0", "80", "0"},
    { "commodore1541", "Writes Commodore 64 disks in 1541 format", "*.d64", "commodore1541.d64", "0", "39", "0"},
    { "commodore1581", "Writes Commodore 64 disks in 1581 format", "*.d81", "commodore1581.d64", "0", "79", "0-1"},
    { "hplif770", "Writes 770 kB HP LIF format disks", "*.img *.imd *.st", "hplif770.img", "0", "76", "0-1"},
    { "ibm1440", "Writes 1440 kB IBM format disks", "*.img *.imd *.st", "ibm1440.img", "0", "79", "0-1"},
    { "ibm720", "Writes 720 kB IBM format disks", "*.img *.imd *.st", "ibm720.img", "0", "79", "0-1"},
    { "macintosh", "Writes Apple Macintosh disks", "*.diskcopy", "macintosh.diskcopy", "0", "79", "0-1"},
    { "tids990", "Writes Texas Instruments DS990 disks", "*.img", "tids990.img", "0", "76", "0-1"},
};

wizard::wizard(QWidget *parent, int intDrive)
     : QWizard(parent)
{
  setPage(Page_Intro, new IntroPage);
  setPage(Page_Read, new ReadPage());
  setPage(Page_Write, new WritePage);
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
        //fluxengine::setAddress("write");
       //parent->plainTextEdit->setPlainText("write");
 //       registerField("fluxenginecommand", "write");
        return wizard::Page_Write;
    } else {
        //fluxengine::setAddress("read");
        return wizard::Page_Read;
    }
}

ReadPage::ReadPage(QWidget *parent)
    : QWizardPage(parent)
{
    QValidator *validator = new QIntValidator(10, 99, this);
    QRegularExpression rx("^[0-1][-][1]$");
    QValidator *validatorhead = new QRegularExpressionValidator(rx, this);
    setTitle(tr("Set the read options for <i>Fluxengine</i>;"));
    setSubTitle(tr("Please choose the format of the disk to be read "

                   "and the name of the output file (e.g., wordperfect5.img)."));

    nameLabel = new QLabel(tr("F&ormat:"));
    readFormatbox = new QComboBox();
    for (unsigned i = 0; i<readformats ; i++) {
       readFormatbox->addItem(my_readformats[i].strDescription);
    }
    //connect(readFormatbox, QOverload<int>::of(&QComboBox::currentIndexChanged()), SLOT(updatedirectorybox()));

    QObject::connect(readFormatbox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatedirectorybox(int)));
    //readFormatbox->setCurrentIndex(0);                                                         //set IBM as standard

    nameLabel->setBuddy(readFormatbox);


    trackLabelStart = new QLabel(tr("Tracks:"));
    trackLineEditStart = new QLineEdit;
    trackLineEditStart->setValidator(validator);
    trackLabelStop = new QLabel(tr(" : "));
    trackLineEditStop = new QLineEdit;
    trackLineEditStop->setValidator(validator);
    directoryComboBox = new QComboBox();

    // -s :d=0:s=0:t=0-39
    HeadLineLabel = new QLabel(tr("Head(s):"));
    HeadLineEdit = new QLineEdit;
    HeadLineEdit->setValidator(validatorhead);

    label = new QLabel("Outputfile");
    button = new QPushButton("Browse");
    connect(button, SIGNAL(clicked()), SLOT(browse()));
    label1 = new QLabel("Write Flux File");
    button1 = new QPushButton("Browse");
    fluxComboBox = new QComboBox();
    connect(button1, SIGNAL(clicked()), SLOT(browseflux()));

    label2 = new QLabel("Use Flux File");
    button2 = new QPushButton("Browse");
    flux1ComboBox = new QComboBox();
    connect(button2, SIGNAL(clicked()), SLOT(browsereadflux()));

    registerField("ReadPage.format", readFormatbox);
    registerField("ReadPage.TrackStart", trackLineEditStart);
    registerField("ReadPage.TrackStop", trackLineEditStop);
    registerField("ReadPage.SaveOutput*", directoryComboBox);
    registerField("ReadPage.Saveflux", fluxComboBox);
    registerField("ReadPage.Save1flux", flux1ComboBox);
    registerField("ReadPage.Heads", HeadLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(readFormatbox, 0, 1);
    layout->addWidget(trackLabelStart, 1, 0);
    layout->addWidget(trackLineEditStart, 1, 1);
    layout->addWidget(trackLabelStop, 1, 2);
    layout->addWidget(trackLineEditStop, 1, 3);
    layout->addWidget(HeadLineLabel, 2, 0);
    layout->addWidget(HeadLineEdit, 2, 1);
    layout->addWidget(label,3,0);
    layout->addWidget(directoryComboBox, 3,1);
    layout->addWidget(button, 3, 2);
    layout->addWidget(label1,4,0);
    layout->addWidget(fluxComboBox, 4,1);
    layout->addWidget(button1, 4, 2);
    layout->addWidget(label2,5,0);
    layout->addWidget(flux1ComboBox, 5,1);
    layout->addWidget(button2, 5, 2);

    setLayout(layout);
}

void ReadPage::initializePage()
{
    readFormatbox->setCurrentIndex(ReadFormatDefault);                                                         //set IBM as standard

}

int ReadPage::nextId() const
{
    _strOutputfile = (directoryComboBox->currentText());
    _strFluxFile = fluxComboBox->currentText();
    _strInputFluxFile = flux1ComboBox->currentText();
    return wizard::Page_Conclusion;
}
void ReadPage::updatedirectorybox(int index)
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    if (index != 0)
    {
        trackLineEditStart->setText(my_readformats[index].trackstart);

        QString drivetext = "drive" + QString::number(intSelectedDrive)+ "40track";
        if (settings.value(drivetext).toBool())
            //40track drive
        {
            trackLineEditStop->setText("39");

        } else
        {
            trackLineEditStop->setText(my_readformats[index].trackstop);
        }
        HeadLineEdit->setText(my_readformats[index].Heads);
    }

    strFilter = my_readformats[readFormatbox->currentIndex()].strFilter;

    strFile =  "/" + my_readformats[readFormatbox->currentIndex()].strDefaultfilenaam;
    QString directory = settings.value("datalocation").toString();
    directoryComboBox->clear();
    if (directory != "")
    {
        if (directoryComboBox->findText(directory + strFile) == -1)
            directoryComboBox->addItem(directory + strFile);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory + strFile));

    } else
    {
        if (directoryComboBox->findText(QDir::currentPath() + strFile) == -1)
            directoryComboBox->addItem(QDir::currentPath() + strFile);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(QDir::currentPath() + strFile));
    }
}

void ReadPage::browse()
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFilter = my_readformats[readFormatbox->currentIndex()].strFilter;
    strFile =  "/" + my_readformats[readFormatbox->currentIndex()].strDefaultfilenaam;
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
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}

void ReadPage::browseflux()
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFile = my_readformats[readFormatbox->currentIndex()].strDefaultfilenaam;
    QString desired =  "/" + strFile.mid(0,strFile.indexOf(".")) + ".flux";
    //QFileDialog dialog(this);
    strFilter = my_readformats[readFormatbox->currentIndex()].strFilter;
    //dialog.setOption(QFileDialog::DontUseNativeDialog, true);
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
        if (fluxComboBox->findText(directory) == -1)
            fluxComboBox->addItem(directory);
        fluxComboBox->setCurrentIndex(fluxComboBox->findText(directory));
    }
}

void ReadPage::browsereadflux()
{
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFile = my_readformats[readFormatbox->currentIndex()].strDefaultfilenaam;
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
        if (flux1ComboBox->findText(directory) == -1)
            flux1ComboBox->addItem(directory);
        flux1ComboBox->setCurrentIndex(flux1ComboBox->findText(directory));
    }
}

WritePage::WritePage(QWidget *parent)
    : QWizardPage(parent)
{
    QValidator *validator = new QIntValidator(10, 99, this);
//    QValidator *validator = new QIntValidator(10, 99, this);
    QRegularExpression rx("^[0-1][-][1]$");
    QValidator *validatorhead = new QRegularExpressionValidator(rx, this);
    setTitle(tr("Write a Disk with <i>Fluxengine</i>;"));
    setSubTitle(tr("Choose the type of disk you want to write and the name of the file you want to write to disk."));

    nameLabel = new QLabel(tr("F&ormat:"));
    writeFormatbox = new QComboBox();
    for (unsigned i = 0; i<writeformats ; i++) {
       writeFormatbox->addItem(my_writeformats[i].strDescription);
    }
    connect(writeFormatbox, SIGNAL(currentIndexChanged(int)), SLOT(Update(int)));
    nameLabel->setBuddy(writeFormatbox);

    trackLabelStart = new QLabel(tr("Tracks:"));
    trackLineEditStart = new QLineEdit;
    trackLineEditStart->setText("0");
    trackLineEditStart->setValidator(validator);
    trackLabelStop = new QLabel(tr(" : "));
    trackLineEditStop = new QLineEdit;
    trackLineEditStop->setText("79");
    trackLineEditStop->setValidator(validator);
    directoryComboBox = new QComboBox();

    // -s :d=0:s=0:t=0-39
    HeadLineLabel = new QLabel(tr("Head(s):"));
    HeadLineEdit = new QLineEdit;
    HeadLineEdit->setText("0-1");
    HeadLineEdit->setValidator(validatorhead);

    label = new QLabel("Inputfile");
    button = new QPushButton("Browse");
    connect(button, SIGNAL(clicked()), SLOT(browse()));
    registerField("WritePage.format", writeFormatbox);
    registerField("WritePage.TrackStart", trackLineEditStart);
    registerField("WritePage.TrackStop", trackLineEditStop);
    registerField("WritePage.OpenInput*", directoryComboBox);
    registerField("WritePage.Heads", HeadLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(writeFormatbox, 0, 1);
    layout->addWidget(trackLabelStart, 1, 0);
    layout->addWidget(trackLineEditStart, 1, 1);
    layout->addWidget(trackLabelStop, 1, 2);
    layout->addWidget(trackLineEditStop, 1, 3);
    layout->addWidget(HeadLineLabel, 2, 0);
    layout->addWidget(HeadLineEdit, 2, 1);
    layout->addWidget(label,3,0);
    layout->addWidget(directoryComboBox, 3,1);
    layout->addWidget(button, 3, 2);
    setLayout(layout);
}

void WritePage::initializePage()
{
    writeFormatbox->setCurrentIndex(WriteFormatDefault);   //set IBM as standard
}

int WritePage::nextId() const
{
    return wizard::Page_Conclusion;

}

void WritePage::browse()
{
    QString strFilter;
    QString strFile;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    strFilter = my_writeformats[writeFormatbox->currentIndex()].strFilter;
    strFile = "/" + my_writeformats[writeFormatbox->currentIndex()].strDefaultfilenaam;

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
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
        _strInputfile = (directoryComboBox->currentText());
    }
}

void WritePage::Update(int index)
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    if (index != 0)
    {
        trackLineEditStart->setText(my_writeformats[index].trackstart);

        QString drivetext = "drive" + QString::number(intSelectedDrive)+ "40track";
        if (settings.value(drivetext).toBool())
            //40track drive
        {
            trackLineEditStop->setText("39");

        } else
        {
            trackLineEditStop->setText(my_writeformats[index].trackstop);
        }
        HeadLineEdit->setText(my_writeformats[index].Heads);
    }
}


ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);

    bottomLabel->setText("");

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
    qInfo() << field("IntroPage.read").toString();
    if (field("IntroPage.read") == "True")
    {
        setTitle(tr("The fluxengine wizard had gathered all the necessary settings to proceed reading the disk."));
        setSubTitle(tr("Press finish to start reading"));
    } else
    {
        setTitle(tr("The fluxengine wizard had gathered all the necessary settings to proceed writing the disk."));
        setSubTitle(tr("Press finish to start writing"));
    }

}

QString wizard::getData()
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
//       qInfo() << field("ReadPage.format").toInt();
       int index = field("ReadPage.format").toInt();

       strDisk = my_readformats[index].strType;
       strFormat.append(strDisk);
//       qInfo() << strFormat;
       QString TrackStart = field("ReadPage.TrackStart").toString();
       QString TrackStop = field("ReadPage.TrackStop").toString();

       if (_strInputFluxFile != "")
       {
           strFormat.append(" -s " + _strInputFluxFile);
       } else
       {
           strFormat.append(" -s drive:" + QString::number(intSelectedDrive));
       }
       strFormat.append(" --cylinders ");
       strFormat.append(TrackStart);
       strFormat.append("-");
       QString drivetext = "drive" + QString::number(intSelectedDrive)+ "40track";
//       qInfo() << drivetext;
//       qInfo() << ((TrackStop > 39) && (settings.value(drivetext).toBool()));
       if ((TrackStop.toInt() > 39) && (settings.value(drivetext).toBool()))
       {
           //40 track drive with 80track operand use doublestep x2
           TrackStop = TrackStop + "x2";
       }

       strFormat.append(TrackStop);

       QString Heads = field("ReadPage.Heads").toString();
       strFormat.append(" -h " + Heads);

//       qInfo() << field("ReadPage.SaveOutput").toString();
       strFormat.append(" -o ");
       strFormat.append(_strOutputfile);

//read ibm -s :d=0:s=0:t=0-39 --overwrite -f ARK.flux -o ARK.imd'
       if (_strFluxFile != "")
       {
           strFormat.append(" --copy-flux-to ");
           strFormat.append(_strFluxFile);
       }
//       qInfo() << strFormat;
       command.append(strFormat);

    }
    else {
        //./fluxengine write commodore1541 -i uridium.d64 -d drive:0 -c 0-39 -h 0
        strFormat = "write ";
        //qInfo() << field("WritePage.format").toInt();
        int index = field("WritePage.format").toInt();

        strDisk = my_writeformats[index].strType;
        strFormat.append(strDisk);
        strFormat.append(" -i ");
        strFormat.append(_strInputfile);

        QString TrackStart = field("WritePage.TrackStart").toString();
        QString TrackStop = field("WritePage.TrackStop").toString();

        strFormat.append(" -d drive:" + QString::number(intSelectedDrive));
        strFormat.append(" --cylinders ");
        strFormat.append(TrackStart);
        strFormat.append("-");
        QString drivetext = "drive" + QString::number(intSelectedDrive)+ "40track";
//        qInfo() << drivetext;
//        qInfo() << ((TrackStop > 39) && (settings.value(drivetext).toBool()));
        if (((TrackStop.toInt()) > 39) && (settings.value(drivetext).toBool()))
        {
            //40 track drive with 80track operand use doublestep x2
            TrackStop = TrackStop + "x2";
        }
        strFormat.append(TrackStop);

        QString writeHeads = field("WritePage.Heads").toString();
        strFormat.append(" -h " + writeHeads);

//        qInfo() << strFormat;
//        qInfo() << field("WritePage.OpenInput").toString();
//        qInfo() << strFormat;
        command.append(strFormat);
    }
    return command;
}
