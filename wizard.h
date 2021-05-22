#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include <QtWidgets>

class wizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Intro , Page_Read, Page_Write,
                 Page_Conclusion};

    wizard(QWidget *parent = 0);

public slots:
    QString getData(int intDrive);

private slots:
    void showHelp();

private:


};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = nullptr);

    int nextId() const override;

private:
    QLabel *topLabel;
    QRadioButton *readRadioButton;
    QRadioButton *writeRadioButton;
};
//! [4]

//! [5]
class ReadPage : public QWizardPage
{
    Q_OBJECT

public:
    ReadPage(QWidget *parent = nullptr);

    int nextId() const override;
    void initializePage() override;

private slots:
    void browse();
    void browseflux();
    void updatedirectorybox(int index);
private:
    QLabel *nameLabel;
    QLabel *trackLabel;
    QLabel *HeadLineLabel;
    QLabel *trackLabelStart;
    QLabel *trackLabelStop;
    QLabel *label;
    QLabel *label1;

    QPushButton *button;
    QPushButton *button1;
    QComboBox *readFormatbox;
    QComboBox *directoryComboBox;
    QComboBox *fluxComboBox;
    QLineEdit *nameLineEdit;
    QLineEdit *trackLineEdit;
    QLineEdit *HeadLineEdit;
    QLineEdit *trackLineEditStart;
    QLineEdit *trackLineEditStop;    

};
//! [5]

class WritePage : public QWizardPage
{
    Q_OBJECT

public:
    WritePage(QWidget *parent = nullptr);

    int nextId() const override;
private slots:
    void browse();
    void Update(int intIndex);

private:
    QLabel *nameLabel;
    QLabel *trackLabel;
    QLabel *HeadLineLabel;
    QLabel *trackLabelStart;
    QLabel *trackLabelStop;
    QLabel *label;
    QLabel *DiskLabel;
    QLabel *namePresetLabel;
    QPushButton *button;
    QComboBox *writeFormatbox;
    QComboBox *directoryComboBox;
    QComboBox *writePresetbox;
    QCheckBox *DiskLineEdit;
    QLineEdit *nameLineEdit;
    QLineEdit *trackLineEdit;
    QLineEdit *HeadLineEdit;
    QLineEdit *trackLineEditStart;
    QLineEdit *trackLineEditStop;
};


//! [6]
class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = nullptr);

    void initializePage() override;
    int nextId() const override;

private:
    QCheckBox *writeFluxFilecheckBox;
    QCheckBox *overwriteFluxFilecheckBox;
    QLabel *bottomLabel;
};
//! [6]

#endif
