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
    Q_UNUSED(error);
    emit output("Error");
}

void fluxengine::finished(int exitcode, QProcess::ExitStatus exitStatus)
{
    if(!m_listening) return;
    Q_UNUSED(exitcode);
    Q_UNUSED(exitStatus);
    emit output("Complete");

}

void fluxengine::readyReadStandardError()
{
    if(!m_listening) return;
    QByteArray data = m_process.readAllStandardError();
    QString message = "Standard Error: ";
    message.append(data);
    if (message.contains("No such file or directory", Qt::CaseInsensitive) || message.contains("Is a directory", Qt::CaseInsensitive))
    {
        //fluxengine could not be located. Set the location to fluxengine
        message = "Error: No such file or directory. Fluxengine could not be located. Use \"Ctrl+P\" to specify where fluxengine can be found.";
    }
    emit output(message);
}

void fluxengine::readyReadStandardOutput()
{
    qInfo() << Q_FUNC_INFO;
//    qInfo() << m_listening;
//    qInfo() << m_process.readAllStandardOutput();
    if(!m_listening) return;
//    if(!m_initializing)
//    {
        QByteArray data = m_process.readAllStandardOutput();
        emit output(QString(data.trimmed()));
//    } else
//    {   //get all the settings form fluxengine
//        m_initializing = false;
//    }
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
        emit enableFluxengineCommands(true);
        break;
    case QProcess::Running:
        startFluxengine();
        emit enableFluxengineCommands(true);
        break;

    }
}

void fluxengine::readyRead()
{

    qInfo() << Q_FUNC_INFO;
    qInfo() << "readyRead: " + m_address;
    if(!m_listening) return;
    QByteArray data = m_process.readAll().trimmed();
    emit output(data);
}

QString fluxengine::getProcess()
{
    if(QSysInfo::productType() == "windows") return "cmd";
    return "bash";

}

void fluxengine::startFluxengine()
{
    QByteArray command;

//    command = (m_workingdirectory + "/fluxengine " + m_address).toUtf8();
    command = (m_workingdirectory + " " + m_address).toUtf8();
    if(QSysInfo::productType() == "windows") command.append("\r");
    command.append("\n");
    m_process.write(command);
}

//void fluxengine::initializeFluxengine()
//{
//    // get all the read formats
//    m_address = "read";
//    start();
//    startFluxengine();
//    foreach (QString x, formats) {
//        qInfo() << Q_FUNC_INFO;
//        m_address = m_address + x + " -C";
//        qInfo() << m_address;
//        startFluxengine();

//    }



//}

//QStringList fluxengine::initializeformats(QByteArray data)
//{
//    QStringList Formats;
//    //returns a stringlist with read or write formats.
//    QSettings settings("Fluxengine_GUI", "Fluxengine_GUI");
//   if (data.contains("syntax: fluxengine read ") || (data.contains("syntax: fluxengine write ")))
//    {
//       /*syntax: fluxengine read <profile> [<options>...]
//        Use --help for option help.
//        Available profiles include:
//          acornadfs
//          acorndfs
//          aeslanier
//          amiga
//          ampro
//          apple2
//          atarist
//          brother
//          commodore1541
//          commodore1581
//          eco1
//          f85
//          fb100
//          ibm
//          macintosh
//          micropolis
//          mx
//          tids990
//          victor9k
//          zilogmcz
//        Or use a text file containing your own configuration.
//        */
//        qInfo() << Q_FUNC_INFO;
//        qInfo() << "read";

//        //get the readformats and store in section
//        int i = data.indexOf(":",0);
//        int j = data.indexOf(":", i+1);
//        for (int x=0; x < data.count(); x++)
//        {
//            i = data.indexOf("\n", j);
//            j = data.indexOf("\n", i+1);
//            if (j == -1) break;
//            qInfo() << j;
//            qInfo() << data.mid(i, j+1 - i).trimmed();
//            Formats.append(data.mid(i, j+1 - i).trimmed());
//            //i = j+1;

//        }
//        return Formats;
////        foreach (QString x, Formats) {
////            qInfo() << x;
////        }
//        //get the config for each readformat and store in section
//        /*
//         * input {
//              flux {
//                drive {
//                }
//              }
//            }
//            output {
//              image {
//                filename: "acornadfs.img"
//                img {
//                }
//              }
//            }
//            decoder {
//              ibm {
//                sector_id_base: 0
//              }
//            }
//            cylinders {
//              start: 0
//              end: 79
//            }
//            heads {
//              start: 0
//              end: 1
//            }
//            */
//        foreach (QString x, Formats) {
//            qInfo() << x;
//        }
//    }
//   return Formats;
//}

//QStringList fluxengine::getConfig(QString data)
//{
//    QStringList Configs;
//    struct FormatsDescription
//    {
//        QString strType;
//        QString strDescription;
//        QString strFilter;
//        QString strDefaultfilenaam;
//        QString trackstart;
//        QString trackstop;
//        QString headstart;
//        QString headstop;
//    };

//    if (data.contains("encoder") || (data.contains("decoder")))
//    {
//        int i = data.indexOf(":",0);
//        int j;
////        for (int x=0; x < data.count(); x++)
////        {
////            i = data.indexOf("\n", );
//            j = data.indexOf("\n", i+1);
////            if (j == -1) break;
//            qInfo() << j;
//            qInfo() << data.mid(i, j+1 - i).trimmed();
//            Configs.append(data.mid(i, j+1 - i).trimmed());
//            //i = j+1;

////        }
//        return Configs;
////        foreach (QString x, Configs) {
////            qInfo() << x;
////        }

//    }
//    //get the config for each readformat and store in section
//    /*
//     * input {
//          flux {
//            drive {
//            }
//          }
//        }
//        output {
//          image {
//            filename: "acornadfs.img"
//            img {
//            }
//          }
//        }
//        decoder {
//          ibm {
//            sector_id_base: 0
//          }
//        }
//        cylinders {
//          start: 0
//          end: 79
//        }
//        heads {
//          start: 0
//          end: 1
//        }
//        */
//    //stringlist contains filename, start cylinder, stop cylinder, start Head, stop head.
//    return Configs;
//}
