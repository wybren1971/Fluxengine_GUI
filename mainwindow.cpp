#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "wizard.h"
#include <qsettings.h>


/*
 * To Do:
 * Aanpassen aan protobuf redesign
 *
 *
 *
 *
 *
*/

bool blnDrive0 = true;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    m_fluxengine.setAddress(ui->plainTextEdit->text());
    connect(ui->btnStop,&QPushButton::clicked,&m_fluxengine,&fluxengine::stop);
    connect(&m_fluxengine,&fluxengine::output,this,&MainWindow::output);
    connect(ui->Fluxengineinput,&QLineEdit::returnPressed,this,&MainWindow::on_pushButton_clicked);
    connect(ui->Fluxengineinput, &QLineEdit::textChanged, this, &MainWindow::buttonenable);
    connect(&m_fluxengine,&fluxengine::enableFluxengineCommands,this,&MainWindow::enableFluxengineCommands);
    //ui->Fluxengineinput.textChanged().connect(lambda: ui->pushButton.setEnabled(ui-Fluxengineinput.textChenged.text() != ""));
    createActions();
    createMenus();
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
    m_fluxengine.setWorkingDirectory(settings.value("fluxengine").toString());

    setWindowTitle(tr("Fluxengine-GUI"));
    this->setFixedSize(QSize(800, 600));
    ui->btnReadDisk->setFocus();
}
//! [2]

//! [3]
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
//! [3]

void MainWindow::newFile()
{
//    infoLabel->setText(tr("Invoked <b>File|New</b>"));
    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory where fluxengine resides"),
                                                QDir::currentPath(),
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    m_fluxengine.setWorkingDirectory(dir);
    settings.setValue("fluxengine", dir);

}

void MainWindow::readdisk()
{
//    infoLabel->setText(tr("Invoked <b>File|Read</b>"));
//    qInfo() << Q_FUNC_INFO;
    if (m_fluxengine.busy())
         m_fluxengine.stop();

    ui->plainTextEdit->clear();
      //gtk_window_set_transient_for();
    wizard Fwizard(this);

    if (Fwizard.exec() == QDialog::Accepted)
//        qInfo() << "Accepted";
//    qInfo() << Fwizard.hasVisitedPage(3);
    if (Fwizard.hasVisitedPage(3))
    {
//       qInfo() << "hasvisitedpage 3";
       if (ui->btnDrive0->isChecked())
       {
            ui->plainTextEdit->setText(Fwizard.getData(0));
       } else
       {
           ui->plainTextEdit->setText(Fwizard.getData(1));
       }
        m_fluxengine.start();
    }
}


void MainWindow::preference()
{
//    infoLabel->setText(tr("Invoked <b>File|Print</b>"));
    //for settings implement QSettings example

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
 //   infoLabel->setText(tr("Invoked <b>Help|About</b>"));
    QMessageBox::about(this, tr("About Menu"),
            tr("This is the fluxengine-GUI created by Wybren van Duinen. "
               "               23 mei 2021"));
}

//! [4]
void MainWindow::createActions()
{
//! [5]
    newAct = new QAction(tr("&Set location of Fluxengine"), this);
    newAct->setShortcuts(QKeySequence::Save);
    newAct->setStatusTip(tr("Set the working directory of fluxengine"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
//! [4]

    openAct = new QAction(tr("&Fluxengine wizard..."), this);
    openAct->setShortcuts(QKeySequence::Find);
    openAct->setStatusTip(tr("Read or write a disk"));
    connect(openAct, &QAction::triggered, this, &MainWindow::readdisk);
//! [5]

//    preferenceAct = new QAction(tr("&Preference..."), this);
//    preferenceAct->setShortcuts(QKeySequence::Print);
//    preferenceAct->setStatusTip(tr("Set the preferences"));
//    connect(preferenceAct, &QAction::triggered, this, &MainWindow::preference);

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

//! [6] //! [7]
    alignmentGroup = new QActionGroup(this);
    alignmentGroup->addAction(leftAlignAct);
    alignmentGroup->addAction(rightAlignAct);
    alignmentGroup->addAction(justifyAct);
    alignmentGroup->addAction(centerAct);
    leftAlignAct->setChecked(true);
//! [6]
}
//! [7]

//! [8]
void MainWindow::createMenus()
{
//! [9] //! [10]
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
//! [9]
    fileMenu->addAction(openAct);
//! [10]
//    fileMenu->addAction(preferenceAct);
//! [11]
    fileMenu->addSeparator();
//! [11]
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);


//! [12]
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
    delete ui;
    m_fluxengine.stop();
}

void MainWindow::output(QString data)
{
    ui->txtOutput->appendPlainText(data);
}

void MainWindow::enableFluxengineCommands(bool blnStarted)
{
    qInfo() << Q_FUNC_INFO;
    qInfo() << blnStarted;

    if (blnStarted)
    {
        ui->Fluxengineinput->setEnabled(true);
        ui->btnStop->setEnabled(true);
//        ui->bntStartFluxengine->setEnabled(false);
    } else
    {
        ui->Fluxengineinput->setEnabled(false);
        ui->btnStop->setEnabled(false);
//        ui->bntStartFluxengine->setEnabled(true);

    }
}

void MainWindow::on_plainTextEdit_textChanged()
{
    m_fluxengine.setAddress(ui->plainTextEdit->text());
}


void MainWindow::on_btnReadDisk_clicked()
{
    qInfo() << Q_FUNC_INFO;
    //start the wizard to create the read command
    //fluxengine read c64 -o c64-test-1.d64
   MainWindow::readdisk();

}


void MainWindow::on_btntestVoltages_clicked()
{
    if (m_fluxengine.busy())
        m_fluxengine.stop();
    m_fluxengine.setAddress("test voltages");
    ui->plainTextEdit->setText(m_fluxengine.getAddress());
    m_fluxengine.start();
}


void MainWindow::on_btntestbandwidth_clicked()
{
    if (m_fluxengine.busy())
         m_fluxengine.stop();
    m_fluxengine.setAddress("test bandwidth");
    ui->plainTextEdit->setText(m_fluxengine.getAddress());
    m_fluxengine.start();

}


void MainWindow::on_btnRPM_clicked()
{
    if (blnDrive0 == true)
    {
        m_fluxengine.setAddress("rpm -s drive:0");
        ui->plainTextEdit->setText(m_fluxengine.getAddress());
    } else
    {
        m_fluxengine.setAddress("rpm -s drive:1");
        ui->plainTextEdit->setText(m_fluxengine.getAddress());
    }
    if (m_fluxengine.busy())
         m_fluxengine.stop();
    m_fluxengine.start();
}


void MainWindow::on_btnDrive0_clicked()
{
    blnDrive0 = true;
}


void MainWindow::on_btnDrive1_clicked()
{
    blnDrive0 = false;
}


void MainWindow::on_bntStartFluxengine_clicked()
{
    if (m_fluxengine.busy())
         m_fluxengine.stop();
    m_fluxengine.start();

}


void MainWindow::on_pushButton_clicked()
{
   // QString input;
    QByteArray Input;
    //input = ui->Fluxengineinput->text();
    if ((ui->Fluxengineinput->text() != "") and  (m_fluxengine.busy()))
    {
        Input = ui->Fluxengineinput->text().toUtf8();
        m_fluxengine.write(Input);
        ui->Fluxengineinput->clear();
    }
}


void MainWindow::on_Fluxengineinput_returnPressed()
{
    //action is handled in the on_pushbutton_clicked()
}


void MainWindow::buttonenable()
{
    if (((ui->Fluxengineinput->text()) != "")  and  (m_fluxengine.busy()))
    {
        ui->pushButton->setEnabled(true);
     } else
    {
        ui->pushButton->setEnabled(false);
    }
}
