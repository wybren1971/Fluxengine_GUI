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

    m_workingdirectory = QDir::toNativeSeparators(Dir);
}

QString fluxengine::getWorkingDirectory()
{
    return m_workingdirectory;
}

void fluxengine::start()
{
    m_listening = true;
    QString program = getProcess();
    QStringList Arguments;
    if(QSysInfo::productType() == "windows") Arguments << "/Q"; // /Q turns echo of on windows.

    m_process.start(program, Arguments, QIODevice::ReadWrite);

}

void fluxengine::startdirect()
{
    m_listening = true;
    QString program = getProcess();
    QByteArray command;
    command = (m_workingdirectory + " " + m_address).toUtf8();
    if(QSysInfo::productType() == "windows") command.append("\r");
    command.append("\n");
    QStringList Arguments;
    if(QSysInfo::productType() == "windows")
    {
        Arguments << "/c" << command;

    } else
    {
        Arguments << "-c" << command;
    }
    m_process.start(program, Arguments, QIODevice::ReadWrite);
}

void fluxengine::stop()
{
    m_listening = false;
    if(QSysInfo::productType() == "windows")
    {
        //on windows fluxengine starts as a separate process and cannot be stopped bij killing the cmd...
        m_process.terminate();
        m_process.kill();
    } else
    {
        m_process.close();
    }
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
    if(!m_listening) return;
    Q_UNUSED(exitcode);
    Q_UNUSED(exitStatus);
    m_listening = false;
    QString message = "   ";
    emit output(message);
}

void fluxengine::readyReadStandardError()
{
    if(!m_listening) return;
    QByteArray data = m_process.readAllStandardError();
    if (data.size() > 1)
    {
        QString message = "";
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
    if(!m_listening) return;
    QByteArray data = m_process.readAllStandardOutput();
    if (data.size() > 1)
    {
//        data.append("readyReadStandardOutput");
        emit output(QString(data.trimmed()));
    }
}

void fluxengine::started()
{
    //do nothing

}

void fluxengine::stateChanged(QProcess::ProcessState newState)
{
    switch (newState) {
    case QProcess::NotRunning:
        emit enableFluxengineCommands(false);
        break;
    case QProcess::Starting:
        break;
    case QProcess::Running:
        if(QSysInfo::productType() == "windows")
        {
            QByteArray command = "prompt $g\r\n";
            m_process.write(command); //get rid of the prompt in windows
        }
        startFluxengine();
        emit enableFluxengineCommands(true);
        break;
    }
}

void fluxengine::readyRead()
{

    if(!m_listening) return;
    QByteArray data = m_process.readAll().trimmed();

    // In windows cmd shows first the windows version and copyright. we dont need to show these.
    //also the exit command is shown. No need to worry the user with that.
    if(QSysInfo::productType() == "windows")
    {
        if (data.contains("Microsoft Corporation."))
            data = "";
        if (data.right(4) == "exit")
            data = "";
    }
    data = QDir::toNativeSeparators(data).toUtf8();
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

    if(QSysInfo::productType() == "windows")
    {
        command = (QStringLiteral("\"") + m_workingdirectory + ("\" " + m_address)).toUtf8();
    } else
    {
        command = (m_workingdirectory + " " + m_address).toUtf8();
    }
//    qInfo() << "command: " << command;
    if(QSysInfo::productType() == "windows") command.append("\r");
    command.append("\n");
    m_process.write(command);
    if (m_address.contains(".imd", Qt::CaseInsensitive) || m_address.isEmpty())
        //give user option to write comment for an imd file
    {
        //dont close fluxengine proces. Fluxengine process will be closed from the mainwindow.
    }   else
    {
        command.clear();
        command.append("exit");
        if(QSysInfo::productType() == "windows") command.append("\r");
        command.append("\n");
        m_process.write(command);
    }
}
