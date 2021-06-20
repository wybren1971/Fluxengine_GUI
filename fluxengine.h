#ifndef FLUXENGINE_H
#define FLUXENGINE_H

#include <QObject>
#include <QDebug>
#include <QSysInfo>
#include <QProcess>

class fluxengine : public QObject
{
    Q_OBJECT
public:
    explicit fluxengine(QObject *parent = nullptr);

    QString operatingSystem();

    QString getAddress() const;
    void setAddress(const QString &address);
    void setWorkingDirectory(const QString &Dir);
    QString getWorkingDirectory();
    void write(QByteArray comment);
signals:
    void output(QString data);
    void enableFluxengineCommands(bool blnStarted);
public slots:
    void start();
    void stop();
    bool busy();

private slots:
    void errorOccured(QProcess::ProcessError error);
    void finished(int exitcode, QProcess::ExitStatus exitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();
    void stateChanged(QProcess::ProcessState newState);
    void readyRead();

private:
    QProcess m_process;
    QString m_address;
    QString m_workingdirectory;
    bool m_listening;
    QString getProcess();
    void startFluxengine();
    void initializeFluxengine();
    QStringList initializeformats(QByteArray data);
    QStringList getConfig(QString data);
};

#endif // FLUXENGINE_H
