#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "wizard.h"
#include <qsettings.h>
#include <dialogpreferences.h>
#include <QApplication>

class ImageViewer : public QWidget {
   QGridLayout m_layout{this};
   QScrollArea m_area;
   QLabel m_imageLabel, m_scaleLabel;
   QPushButton m_zoomOut{"Zoom Out"}, m_zoomIn{"Zoom In"};
   double m_scaleFactor = 1.0;
public:
   void setImage(const QImage & img) {
      m_scaleFactor = 1.0;
      m_imageLabel.setPixmap(QPixmap::fromImage(img));
      scaleImage(m_scaleFactor);
      this->setMinimumWidth(600);
      this->setMinimumHeight(480);
      this->resize(850,660);

   }
   ImageViewer() {
      m_layout.addWidget(&m_area, 0, 0, 1, 3);
      m_layout.addWidget(&m_zoomOut, 1, 0);
      m_layout.addWidget(&m_scaleLabel, 1,1, Qt::AlignCenter);
      m_layout.addWidget(&m_zoomIn, 1, 2);
      m_area.setWidget(&m_imageLabel);
      m_imageLabel.setScaledContents(true);
      connect(&m_zoomIn, &QPushButton::clicked, [this]{ scaleImage(1.1); });
      connect(&m_zoomOut, &QPushButton::clicked, [this]{ scaleImage(1.0/1.1); });
   }
   void scaleImage(double factor) {
      m_scaleFactor *= factor;
      m_scaleLabel.setText(QStringLiteral("%1%").arg(m_scaleFactor*100, 0, 'f', 1));
      QSize size = m_imageLabel.pixmap(Qt::ReturnByValue).size() * m_scaleFactor;
      m_imageLabel.resize(size);
   }
};

bool blnFirsttime;

int NUMBER_OF_COMMANDS;
/*
 * To Do:
 *
 * Vastleggen van type drives in preferences zodat goede settings gebruikt worden (40track HD 96tpi, 48 tpi ertc)
 *
 *
 *
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    m_fluxengine.setWorkingDirectory(settings.value("fluxengine").toString());
    if (settings.value("NUMBER_OF_COMMANDS").toString() == "")
    {//set default
        NUMBER_OF_COMMANDS = 10;
    } else
    {
        NUMBER_OF_COMMANDS = settings.value("NUMBER_OF_COMMANDS").toInt();
    }
    restoreGeometry(settings.value("myWidget/geometry").toByteArray());
    restoreState(settings.value("myWidget/windowState").toByteArray());
    ui->setupUi(this);
    connect(ui->btnStop,&QPushButton::clicked,&m_fluxengine,&fluxengine::stop);
//    QObject::connect(qApp, &QGuiApplication::applicationStateChanged, this, [=](Qt::ApplicationState state){qDebug() << state;});
    connect(&m_fluxengine,&fluxengine::output,this,&MainWindow::output);
    connect(ui->Fluxengineinput,&QLineEdit::returnPressed,this,&MainWindow::on_pushButton_clicked);
    connect(ui->Fluxengineinput, &QLineEdit::textChanged, this, &MainWindow::buttonenable);
    connect(ui->plainTextEdit_2->lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_bntStartFluxengine_clicked()));
    connect(&m_fluxengine,&fluxengine::enableFluxengineCommands,this,&MainWindow::enableFluxengineCommands);
    ui->txtOutput->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->txtOutput,&QPlainTextEdit::customContextMenuRequested,this,&MainWindow::showContextMenu);

    createActions();
    createMenus();
    setDrive();
    setWindowTitle(tr("Fluxengine-GUI"));
    if (settings.value("fluxengine").toString() == "")
        blnFirsttime = true;

    int width = settings.value("WindowWidth").toInt();
    int height = settings.value("WindowHeight").toInt();
    if (width == 0)
        width = 800;
    if (height == 0)
        height = 600;
    this->resize(width, height);
    if (settings.value("showanalyzebutton").toString() == "")
    {
        ui->btnAnalyse->setVisible(false);
    } else
    {
        ui->btnAnalyse->setVisible(settings.value("showanalyzebutton").toBool());
    }
    if (settings.value("showinspectbutton").toString() == "")
    {
        ui->btnInspect->setVisible(false);
    } else
    {
        ui->btnInspect->setVisible(settings.value("showinspectbutton").toBool());
    }
    ui->btnReadDisk->setFocus();
    ui->plainTextEdit_2->completer();
    ui->plainTextEdit_2->setInsertPolicy(QComboBox::NoInsert);
    ui->txtOutput->setStyleSheet("QPlainTextEdit { font: 8pt Monospace ; background-color: #000; color: white; }"
                                 "QToolTip { color: #ffffff; background-color: #000000; border: 0px; }"
                                 "QMenu { color: #ffffff; background-color: #000000; border: 0px; }");

    ReadItemList();
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(cutAct);
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::newFile()
{

}

void MainWindow::readdisk()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    if (!firsttimecheck("wizard"))
        return;
//    qInfo() << "readinfo rest";
    int intDrive;
    if (ui->btnDrive0->isChecked())
    {
        intDrive= 0;
    } else
    {
        intDrive= 1;
    }

    wizard Fwizard(this, intDrive);


    if (Fwizard.exec() == QDialog::Accepted)
    {
        if (Fwizard.hasVisitedPage(4))
        {
            m_fluxengine.setAddress(Fwizard.getData());
            //see if something needs to be added to the combobox
            if (ui->plainTextEdit_2->findText(Fwizard.getData()) == -1)
            {
                ui->plainTextEdit_2->addItem(Fwizard.getData());
                ui->plainTextEdit_2->setCurrentIndex(ui->plainTextEdit_2->findText(Fwizard.getData()));
                 WriteItemList();
            } else
            {
               ui->plainTextEdit_2->setCurrentIndex(ui->plainTextEdit_2->findText(Fwizard.getData()));
            }
           m_fluxengine.start();
           callingfunction = "readdisk()";
        }
    }
}


void MainWindow::preference()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    DialogPreferences *form = new DialogPreferences();
    form->setWindowTitle("Preferences Fluxengine_GUI");
    form->exec();
    setDrive();
    ui->btnAnalyse->setVisible(settings.value("showanalyzebutton") == "true");
    ui->btnInspect->setVisible(settings.value("showinspectbutton") == "true");

    if (settings.value("fluxengine").toString() != "")
        m_fluxengine.setWorkingDirectory(settings.value("fluxengine").toString());
    NUMBER_OF_COMMANDS = settings.value("NUMBER_OF_COMMANDS").toInt();
    ReadItemList();
}

void MainWindow::undo()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Undo</b>"));
}

void MainWindow::redo()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Redo</b>"));
}

void MainWindow::cut()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Cut</b>"));
}

void MainWindow::copy()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Copy</b>"));
}

void MainWindow::paste()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Paste</b>"));
}

void MainWindow::bold()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Bold</b>"));
}

void MainWindow::italic()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Italic</b>"));
}

void MainWindow::leftAlign()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Left Align</b>"));
}

void MainWindow::rightAlign()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Right Align</b>"));
}

void MainWindow::justify()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Justify</b>"));
}

void MainWindow::center()
{
//    infoLabel->setText(tr("Invoked <b>Edit|Format|Center</b>"));
}

void MainWindow::setLineSpacing()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Format|Set Line Spacing</b>"));
}

void MainWindow::setParagraphSpacing()
{
 //   infoLabel->setText(tr("Invoked <b>Edit|Format|Set Paragraph Spacing</b>"));
}

void MainWindow::about()
{
    QString yourAppVersion = QCoreApplication::applicationVersion();

    QString datetime = QStringLiteral(__DATE__) + QStringLiteral(" ") + QStringLiteral(__TIME__);
    QMessageBox::about(this, tr("About Menu"),
            tr("This is the fluxengine-GUI created by Wybren van Duinen."
               "\nApplication version: ") +yourAppVersion + "\nBuilddate and time: " + datetime);
}

void MainWindow::setDrive()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    if (settings.value("drive0").toString() != "")
    {
        ui->btnDrive0->setText("0: " + settings.value("drive0").toString());
    }
    if (settings.value("drive1").toString() != "")
    {
        if (settings.value("drive1").toString() == "No drive 1 present")
        {
            ui->groupBox->hide();
        } else
        {
            ui->groupBox->show();
            ui->btnDrive1->setText("1: " + settings.value("drive1").toString());
        }
    }
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Fluxengine wizard..."), this);
    openAct->setShortcuts(QKeySequence::Find);
    openAct->setStatusTip(tr("Read or write a disk"));
    connect(openAct, &QAction::triggered, this, &MainWindow::readdisk);

    clear = new QAction(tr("Clear &output fluxengine"), this);
//    clear->setShortcuts(QKeySequence::Find);
    clear->setStatusTip(tr("Clear the box with the output from fluxengine"));
    connect(clear, &QAction::triggered, this, &MainWindow::ClearText);


    preferenceAct = new QAction(tr("&Preference..."), this);
    preferenceAct->setShortcuts(QKeySequence::Print);
    preferenceAct->setStatusTip(tr("Set the preferences"));
    connect(preferenceAct, &QAction::triggered, this, &MainWindow::preference);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last operation"));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);

    boldAct = new QAction(tr("&Bold"), this);
    boldAct->setCheckable(true);
    boldAct->setShortcut(QKeySequence::Bold);
    boldAct->setStatusTip(tr("Make the text bold"));
    connect(boldAct, &QAction::triggered, this, &MainWindow::bold);

    QFont boldFont = boldAct->font();
    boldFont.setBold(true);
    boldAct->setFont(boldFont);

    italicAct = new QAction(tr("&Italic"), this);
    italicAct->setCheckable(true);
    italicAct->setShortcut(QKeySequence::Italic);
    italicAct->setStatusTip(tr("Make the text italic"));
    connect(italicAct, &QAction::triggered, this, &MainWindow::italic);

    QFont italicFont = italicAct->font();
    italicFont.setItalic(true);
    italicAct->setFont(italicFont);

    setLineSpacingAct = new QAction(tr("Set &Line Spacing..."), this);
    setLineSpacingAct->setStatusTip(tr("Change the gap between the lines of a "
                                       "paragraph"));
    connect(setLineSpacingAct, &QAction::triggered, this, &MainWindow::setLineSpacing);

    setParagraphSpacingAct = new QAction(tr("Set &Paragraph Spacing..."), this);
    setParagraphSpacingAct->setStatusTip(tr("Change the gap between paragraphs"));
    connect(setParagraphSpacingAct, &QAction::triggered,
            this, &MainWindow::setParagraphSpacing);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    leftAlignAct = new QAction(tr("&Left Align"), this);
    leftAlignAct->setCheckable(true);
    leftAlignAct->setShortcut(tr("Ctrl+L"));
    leftAlignAct->setStatusTip(tr("Left align the selected text"));
    connect(leftAlignAct, &QAction::triggered, this, &MainWindow::leftAlign);

    rightAlignAct = new QAction(tr("&Right Align"), this);
    rightAlignAct->setCheckable(true);
    rightAlignAct->setShortcut(tr("Ctrl+R"));
    rightAlignAct->setStatusTip(tr("Right align the selected text"));
    connect(rightAlignAct, &QAction::triggered, this, &MainWindow::rightAlign);

    justifyAct = new QAction(tr("&Justify"), this);
    justifyAct->setCheckable(true);
    justifyAct->setShortcut(tr("Ctrl+J"));
    justifyAct->setStatusTip(tr("Justify the selected text"));
    connect(justifyAct, &QAction::triggered, this, &MainWindow::justify);

    centerAct = new QAction(tr("&Center"), this);
    centerAct->setCheckable(true);
    centerAct->setShortcut(tr("Ctrl+E"));
    centerAct->setStatusTip(tr("Center the selected text"));
    connect(centerAct, &QAction::triggered, this, &MainWindow::center);

    alignmentGroup = new QActionGroup(this);
    alignmentGroup->addAction(leftAlignAct);
    alignmentGroup->addAction(rightAlignAct);
    alignmentGroup->addAction(justifyAct);
    alignmentGroup->addAction(centerAct);
    leftAlignAct->setChecked(true);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(preferenceAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(clear);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    formatMenu = editMenu->addMenu(tr("&Format"));
    formatMenu->addAction(boldAct);
    formatMenu->addAction(italicAct);
    formatMenu->addSeparator()->setText(tr("Alignment"));
    formatMenu->addAction(leftAlignAct);
    formatMenu->addAction(rightAlignAct);
    formatMenu->addAction(justifyAct);
    formatMenu->addAction(centerAct);
    formatMenu->addSeparator();
    formatMenu->addAction(setLineSpacingAct);
    formatMenu->addAction(setParagraphSpacingAct);
}

MainWindow::~MainWindow()
{

    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("WindowWidth", MainWindow::width());
    settings.setValue("WindowHeight", MainWindow::height());
    delete ui;
    m_fluxengine.stop();
}

void MainWindow::output(QString data)
{
    if (data.size() > 1)
    {
        ui->txtOutput->appendPlainText(data);
    }
    if (waitforfluzenginetofinish)
    {
        if (callingfunction == "on_pushButton_clicked()")
        {
           on_pushButton_clicked();
        }
        waitforfluzenginetofinish = false;
    }
}

bool MainWindow::firsttimecheck(QString message)
{
    if (blnFirsttime)
    {
        if (message == "")
        {
            message = tr("Welcome to fluxengine_gui\n"
                         "Set the location of fluxengine in preferences and initialize fluxengine on the tab 'My Locations'.\nThese are the minimum steps needed to use the Fluxengine_GUI");
        }
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Fluxengine Wizard Info"), message);

        //if cancel then dont go to preferences
        if (reply == QMessageBox::Yes)
        {
            preference();
        } else
        {
            return false;
        }
        if (m_fluxengine.getWorkingDirectory() != "")
        {
            blnFirsttime = false;
            return true;
        } else
        {
             return false;
        }
    } else
    {
        if (!message.isEmpty())
        {
            QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

            settings.beginGroup("readformats");
            if (!settings.contains("0"))
            {
                //fluxengine not initialized
                message = tr("Welcome to fluxengine_gui\n"
                             "Please initialize fluxengine in preferences on the tab 'My Locations' and press initialize fluxengine.\nThis is needed to use the Wizard");
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, tr("Fluxengine Wizard Info"), message);

                //if cancel then dont go to preferences
                if (reply == QMessageBox::Yes)
                {
                    preference();

                } else
                {
                    return false;
                }
                if (!settings.contains("readformats0"))
                {
                    return false;
                } else
                {
                    return true;
                }
            }
            settings.endGroup();
        } else
        {
            return true;
        }
    }
    return true;
}

void MainWindow::enableFluxengineCommands(bool blnStarted)
{
//    qInfo() << Q_FUNC_INFO;
    if (blnStarted)
    {
        ui->Fluxengineinput->setEnabled(true);
        ui->btnStop->setEnabled(true);
        ui->btnRPM->setEnabled(false);
        ui->btnAnalyse->setEnabled(false);
        ui->btnReadDisk->setEnabled(false);
        ui->btntestVoltages->setEnabled(false);
        ui->btntestbandwidth->setEnabled(false);
        ui->bntStartFluxengine->setEnabled(false);
        ui->btnInspect->setEnabled(false);

        waitforfluzenginetofinish=true;
    } else
    {
        ui->Fluxengineinput->setEnabled(false);
        ui->btnStop->setEnabled(false);
        ui->btnRPM->setEnabled(true);
        ui->btnAnalyse->setEnabled(true);
        ui->btnReadDisk->setEnabled(true);
        ui->btntestVoltages->setEnabled(true);
        ui->btntestbandwidth->setEnabled(true);
        ui->bntStartFluxengine->setEnabled(true);
        ui->btnInspect->setEnabled(true);
        if (waitforfluzenginetofinish)
        {
            if (callingfunction == "on_btnAnalyse_clicked()")
            {
                on_btnAnalyse_clicked();
            }

            if (callingfunction == "on_pushButton_clicked()")
            {
                on_pushButton_clicked();
            }
            waitforfluzenginetofinish=false;
        }
    }
}

void MainWindow::on_btnReadDisk_clicked()
{
   MainWindow::readdisk();
}

void MainWindow::on_btntestVoltages_clicked()
{
    if (!firsttimecheck(""))
        return;
    m_fluxengine.setAddress("test voltages");
    m_fluxengine.start();
    callingfunction = "on_btntestVoltages_clicked()";}


void MainWindow::on_btntestbandwidth_clicked()
{
    if (!firsttimecheck(""))
        return;
    m_fluxengine.setAddress("test bandwidth");
    m_fluxengine.start();
    callingfunction = "on_btntestbandwidth_clicked()";}


void MainWindow::on_btnRPM_clicked()
{
    if (!firsttimecheck(""))
        return;
    if (ui->btnDrive0->isChecked())
    {
        m_fluxengine.setAddress("rpm -s drive:0");
    } else
    {
        m_fluxengine.setAddress("rpm -s drive:1");
    }

    m_fluxengine.start();
    callingfunction = "on_btnRPM_clicked()";
}

void MainWindow::on_btnDrive0_clicked()
{
}

void MainWindow::on_btnDrive1_clicked()
{
}

void MainWindow::ReadItemList()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    ui->plainTextEdit_2->clear();
    for (int i = 0;i<NUMBER_OF_COMMANDS;i++)
    {
        QString setting = "Fluxengine.command";
        QString s = QString::number(i);
        setting = setting + s;
        if (settings.value(setting).toString() != "")
        {
            ui->plainTextEdit_2->addItem(settings.value(setting).toString());
            ui->plainTextEdit_2->setCurrentIndex(i);
        }
    }
}

void MainWindow::WriteItemList()
{
//    qInfo() << Q_FUNC_INFO;
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    QString setting = "Fluxengine.command";

    //write to settings
    QStringList Commands[NUMBER_OF_COMMANDS];
//    qInfo() << ui->plainTextEdit_2->count() << NUMBER_OF_COMMANDS;
    if (ui->plainTextEdit_2->count() < NUMBER_OF_COMMANDS)
    {
        //add item to the list
        ui->plainTextEdit_2->addItem(m_fluxengine.getAddress());
        ui->plainTextEdit_2->setCurrentIndex(ui->plainTextEdit_2->findText(m_fluxengine.getAddress()));
    } else
    {
        //move everything i position up.
        QString settingold = "Fluxengine.command";
        //overwrite move everything up and loose first command.
        //1 overschrijft 0, 2 overschrijft 1 3 overschrijft 2 etc 9 wordt toegevoegd
        for (int i = 0;i<(NUMBER_OF_COMMANDS-1);i++)
        {
            QString s = QString::number(i);     //0
            QString t = QString::number(i+1);   //1
            settings.setValue(settingold + s,settings.value(settingold +t));
        }
        settings.setValue(setting+(QString::number(NUMBER_OF_COMMANDS-1)),m_fluxengine.getAddress());
        ui->plainTextEdit_2->clear();
        ReadItemList();
        ui->plainTextEdit_2->setCurrentIndex(NUMBER_OF_COMMANDS);
    }

    for (int i = 0;i<ui->plainTextEdit_2->count();i++)
    {
        ui->plainTextEdit_2->setCurrentIndex(i);
        Commands->append(ui->plainTextEdit_2->currentText());
    }
    for (int i = 0; i< Commands->size(); i++)
    {
        QString s = QString::number(i);
//        qInfo() << setting << "command: " << Commands->at(i);
        settings.setValue(setting + s, Commands->at(i));
    }
}
void MainWindow::on_bntStartFluxengine_clicked()
{
    if (!firsttimecheck(""))
        return;

    //We dont want the test and rpm commands in the list because there are dedicated buttons for this.
    QString string1 = m_fluxengine.getAddress();
    QString string2 = m_fluxengine.getAddress();
    QString string3 = m_fluxengine.getAddress();
    string1.truncate(4);
    string2.truncate(3);
    string3.truncate(7);
    if (( string1 != "test") && (string2 != "rpm") && (string2 != "analyse"))
    {
//        qInfo() << (ui->plainTextEdit_2->findText(m_fluxengine.getAddress()) == -1);
        if (ui->plainTextEdit_2->findText(m_fluxengine.getAddress()) == -1)
        {
            WriteItemList();
        }
        m_fluxengine.start();
    } else
    {
        //if there is a valid command in the listbox execute that
        if (ui->plainTextEdit_2->currentText() != "")
        {
            m_fluxengine.setAddress(ui->plainTextEdit_2->currentText());
            m_fluxengine.start();
        } else
        {
            //open an empty session
            m_fluxengine.setAddress("");
            m_fluxengine.start();
        }
    }
    callingfunction = "on_bntStartFluxengine_clicked()";

}

void MainWindow::on_pushButton_clicked()
{
//    qInfo() << Q_FUNC_INFO;
    QByteArray Input;
    //input = ui->Fluxengineinput->text();
    if (!waitforfluzenginetofinish)
    {
        if ((ui->Fluxengineinput->text() != "") && (m_fluxengine.busy()))
        {
            waitforfluzenginetofinish = true;
            callingfunction = "on_pushButton_clicked()"; // works with the output signal. the process will give back what it hgas written.
            Input = ui->Fluxengineinput->text().toUtf8();
            m_fluxengine.write(Input);
            ui->Fluxengineinput->clear();
        } else
        {
            //do nothing
        }
    } else
    {
        if (m_fluxengine.getAddress() == "")
        {

        } else
        {
        Input.clear();
        Input.append("exit");
        if(QSysInfo::productType() == "windows") Input.append("\r");
        Input.append("\n");
        m_fluxengine.write(Input);
        }
    }
}

void MainWindow::on_Fluxengineinput_returnPressed()
{
    //action is handled in the on_pushbutton_clicked()
}

void MainWindow::buttonenable()
{
    if (((ui->Fluxengineinput->text()) != "")  &&  (m_fluxengine.busy()))
    {
        ui->pushButton->setEnabled(true);
     } else
    {
        ui->pushButton->setEnabled(false);
    }
}

void MainWindow::on_plainTextEdit_2_editTextChanged(const QString &arg1)
{
    m_fluxengine.setAddress(arg1);
}

void MainWindow::showContextMenu(const QPoint &pt)
{
    QMenu *menu = ui->txtOutput->createStandardContextMenu();
    menu->addAction(clear);
    menu->exec(ui->txtOutput->mapToGlobal(pt));
    delete menu;
}

void MainWindow::ClearText()
{
    ui->txtOutput->clear();
}

void MainWindow::on_btnAnalyse_clicked()
{
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    QDir dir;
    if (!firsttimecheck(""))
        return;

    if (!waitforfluzenginetofinish)
    {
        QString strFilter = "*.csv";
        QString directory = settings.value("csvlocation").toString();
        //let the user choose a csv file
        dir.setPath(QFileDialog::getOpenFileName(this,
                                tr("Choose the csv file to analyze"), directory, strFilter));

        if (dir.absolutePath() != "")
        {

            m_fluxengine.setAddress("analyse layout --csv \"" + dir.absolutePath() + "\"");
            m_fluxengine.start();
            waitforfluzenginetofinish = true;
            callingfunction = "on_btnAnalyse_clicked()";
        }
    } else
    {
        //show the resulting png
        QImage Image(dir.absolutePath() + "/disklayout.png");
        ImageViewer *form = new ImageViewer;
        form->setImage(Image);
        form->setWindowTitle("Analyze: " + dir.absolutePath() + "/disklayout.png");
        form->show();
    }
}

void MainWindow::on_btnInspect_clicked()
{
    if (!firsttimecheck(""))
        return;
    m_fluxengine.setAddress("inspect");
    m_fluxengine.start();
    callingfunction = "on_btnInspect_clicked()";
}
