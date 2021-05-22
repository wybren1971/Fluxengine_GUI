#include "fluxengine.h"
#include <QDir>

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
    //connect(&m_process,&QProcess::finished,this,&fluxengine::finished(int, QProcess::ExitStatus));

    m_listening = false;
    m_address = "";
    m_workingdirectory = QDir::currentPath();

}
void fluxengine::write(QByteArray comment)
{
    qInfo() << Q_FUNC_INFO;
    qInfo() << comment;

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
    //qInfo() << (m_process.error());
    //qInfo() << (m_process.errorString());
    //qInfo() << (m_process.errorOccurred());

    //m_process.start();
//    m_process.start(getProcess(), , ReadWrite);
}

void fluxengine::stop()
{
    qInfo() << Q_FUNC_INFO;
    m_listening = false;
    m_process.close();
}

bool fluxengine::busy()
{
    qInfo() << Q_FUNC_INFO;
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
    qInfo() << Q_FUNC_INFO << error;
    emit output("Error");
}

void fluxengine::finished(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
    Q_UNUSED(exitcode);
    Q_UNUSED(exitStatus);
//    emit output("Complete");

}

void fluxengine::readyReadStandardError()
{
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
    QByteArray data = m_process.readAllStandardError();
    QString message = "Standard Error: ";
    message.append(data);
    if (message.contains("No such file or directory", Qt::CaseInsensitive))
    {
        //fluxengine could not be located. Set the location to fluxengine
        message = "Fluxengine could not be located. Use \"Ctrl+S\" to specify where fluxengine can be found.";
    }
    emit output(message);
}

void fluxengine::readyReadStandardOutput()
{
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
    QByteArray data = m_process.readAllStandardOutput();
    emit output(QString(data.trimmed()));

}

void fluxengine::started()
{
    //do nothing
    qInfo() << Q_FUNC_INFO;

}

void fluxengine::stateChanged(QProcess::ProcessState newState)
{
    qInfo() << Q_FUNC_INFO;
    switch (newState) {
    case QProcess::NotRunning:
//        emit output("Not Running");
        break;
    case QProcess::Starting:
//        emit output("Starting");
        break;
    case QProcess::Running:
//        emit output("Running");
        //startfluxengine
        startFluxengine();
        break;

    }
}

void fluxengine::readyRead()
{
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
    QByteArray data = m_process.readAll().trimmed();
    qInfo() << data;
    emit output(data);

}

QString fluxengine::getProcess()
{
    qInfo() << Q_FUNC_INFO;
    if(QSysInfo::productType() == "windows") return "cmd";

   // if(QSysInfo::productType() == "OSX") return "??";

    return "bash";

}

void fluxengine::startFluxengine()
{
    QByteArray command;

    command = (m_workingdirectory + "/fluxengine " + m_address).toUtf8();
    if(QSysInfo::productType() == "windows") command.append("\r");
    command.append("\n");
    m_process.write(command);
}
