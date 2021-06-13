#include "fluxengine.h"
#include <QDir>
#include <qsettings.h>

fluxengine::fluxengine(QObject *parent) : QObject(parent)
{
    connect(&m_process,&QProcess::errorOccurred,this,&fluxengine::errorOccured);
    connect(&m_process,&QProcess::readyReadStandardError,this,&fluxengine::readyReadStandardError);
    connect(&m_process,&QProcess::readyReadStandardOutput,this,&fluxengine::readyReadStandardOutput);
    connect(&m_process,&QProcess::started,this,&fluxengine::started);
    connect(&m_process,&QProcess::stateChanged,this,&fluxengine::stateChanged);
    connect(&m_process,&QProcess::readyRead,this,&fluxengine::readyRead);

    //finished is overloaded...
    connect(&m_process,QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this,&fluxengine::finished);

    m_listening = false;
    m_address = "";
    m_workingdirectory = QDir::currentPath();
}
void fluxengine::write(QByteArray comment)
{
//    qInfo() << Q_FUNC_INFO;
//    qInfo() << comment;

    if(QSysInfo::productType() == "windows") comment.append("\r");
    m_process.write(comment+"\n");
}

QString fluxengine::operatingSystem()
{
    return QSysInfo::prettyProductName();
}

QString fluxengine::getAddress() const
{
    return m_address;
}

void fluxengine::setAddress(const QString &address)
{
    m_address = address;
}

void fluxengine::setWorkingDirectory(const QString &Dir)
{
    m_workingdirectory = Dir;
}

QString fluxengine::getWorkingDirectory()
{
    return m_workingdirectory;
}

void fluxengine::start()
{
    qInfo() << Q_FUNC_INFO;
    m_listening = true;
    QString program = getProcess();
    QStringList Arguments;
    m_process.start(program, Arguments, QIODevice::ReadWrite);
}

void fluxengine::stop()
{
    m_listening = false;
    m_process.close();
}

bool fluxengine::busy()
{
    if (!m_listening)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void fluxengine::errorOccured(QProcess::ProcessError error)
{
    if(!m_listening) return;
    switch (error) {
        case QProcess::FailedToStart:
            emit output("Error: Failed to start");
            break;
        case QProcess::Crashed:
            emit output("Error: Crashed");
            break;
        case QProcess::Timedout:
            emit output("Error: Timedout");
            break;
        case QProcess::WriteError:
            emit output("Error: Write error");
            break;
        case QProcess::ReadError:
            emit output("Error: Read error");
            break;
        default:
            emit output("Error: Unknown error");
    }
}

void fluxengine::finished(int exitcode, QProcess::ExitStatus exitStatus)
{
    qInfo() << Q_FUNC_INFO;
    qInfo() << exitcode;
    if(!m_listening) return;
    Q_UNUSED(exitcode);
    Q_UNUSED(exitStatus);
    m_listening = false;
    QString message = "  ";
    emit output(message);
}

void fluxengine::readyReadStandardError()
{
    if(!m_listening) return;
    QByteArray data = m_process.readAllStandardError();
    if (data.size() > 1)
    {
        QString message = "Standard Error: ";

        message.append(data);
        if (message.contains("No such file or directory", Qt::CaseInsensitive) || message.contains("Is a directory", Qt::CaseInsensitive))
        {
            //fluxengine could not be located. Set the location to fluxengine
            message = "Error: No such file or directory. Fluxengine could not be located. Use \"Ctrl+P\" to specify where fluxengine can be found.";
        }
        emit output(message);
    }
}

void fluxengine::readyReadStandardOutput()
{
    qInfo() << Q_FUNC_INFO;
    qInfo() << m_process.readAllStandardOutput().size();
    if(!m_listening) return;
    QByteArray data = m_process.readAllStandardOutput();
    if (data.size() > 1)
    {
        emit output(QString(data.trimmed()));
    }
}

void fluxengine::started()
{
    //do nothing

}

void fluxengine::stateChanged(QProcess::ProcessState newState)
{
    qInfo() << Q_FUNC_INFO;
    switch (newState) {
    case QProcess::NotRunning:
        qInfo() << "NotRunning";
        emit enableFluxengineCommands(false);
        break;
    case QProcess::Starting:
        qInfo() << "Starting";
        emit enableFluxengineCommands(false);
        break;
    case QProcess::Running:
        qInfo() << "Running";
        startFluxengine();
        emit enableFluxengineCommands(true);
        break;

    }
}

void fluxengine::readyRead()
{

    qInfo() << Q_FUNC_INFO;
//    qInfo() << "readyRead: " + m_address;
    if(!m_listening) return;
    QByteArray data = m_process.readAll().trimmed();
    if (data.size() > 1)
    {
        emit output(data);
    }
}

QString fluxengine::getProcess()
{
    if(QSysInfo::productType() == "windows") return "cmd";
    return "bash";

}

void fluxengine::startFluxengine()
{
    QByteArray command;

    qInfo() << Q_FUNC_INFO;
    command = (m_workingdirectory + " " + m_address).toUtf8();
    qInfo() << command;
    if(QSysInfo::productType() == "windows") command.append("\r");
    command.append("\n");
    m_process.write(command);
//    m_process.waitForFinished(500);
    command.clear();
    command.append("exit");
    if(QSysInfo::productType() == "windows") command.append("\r");
    command.append("\n");
    m_process.write(command);
}
