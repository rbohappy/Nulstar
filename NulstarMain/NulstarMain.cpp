#include "NMainController.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QHostAddress>
#include <QPair>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include "NModulesManager.h"

const QString lControllerName("ServiceController");
const QString lManageParameter("Managed");
const QString lServiceManagerName("ServiceManager");
const QString lServiceManagerUrlParameter("managerurl");
const QString lConstantsFile("Constants.ncf");

int main(int argc, char* argv[])
{
    QCoreApplication lApp(argc, argv);
    QString lAppName(QString(APP_NAME));
    lApp.setApplicationName(lAppName);
    lApp.setApplicationVersion(APP_VERSION);
    lApp.setOrganizationDomain(QStringLiteral(APP_DOMAIN));
    lApp.setOrganizationName(QStringLiteral(APP_DOMAIN));

    QCommandLineParser lParser;
    lParser.setApplicationDescription(lAppName);
    lParser.addHelpOption();
    lParser.addVersionOption();
    lParser.process(lApp);

    QString lConfigFile;
    QSettings lConstants(lConstantsFile, QSettings::IniFormat);
    lConstants.beginGroup(lControllerName);
    lConfigFile = lConstants.value(QString("ConfigFile")).toString();
    lConstants.endGroup();

    QList<QNetworkAddressEntry> lAllowedNetworks;
    QHostAddress lHostAddress;
    QHostAddress::SpecialAddress lBindAddress;
    QString lCommunicationPort, lLogLevel, lSslModeStr;
    QSettings lSettings(lConfigFile, QSettings::IniFormat);
    QStringList lNetworks, lGroups(lSettings.childGroups());
    lCommunicationPort = lSettings.value(QString("Networks/CommPort")).toString();
    lLogLevel = lSettings.value(QString("Output/LogLevel")).toString();
    lHostAddress.setAddress(lSettings.value(QString("Networks/MainControllerIP")).toString());
    lSslModeStr = lSettings.value(QString("Security/SslMode"), "0").toString();
    lNetworks = lSettings.value(QString("Networks/AllowedNetworks")).toString().split(",");

    for (const QString& lNetwork : lNetworks) {
        QStringList lParams(lNetwork.split("/"));
        QNetworkAddressEntry lNetworkAddress;
        lNetworkAddress.setIp(QHostAddress(lParams.at(0).simplified()));
        if (lParams.size() > 1)
            lNetworkAddress.setPrefixLength(lParams.at(1).simplified().toInt());
        lAllowedNetworks << lNetworkAddress;
    }

    NulstarNS::NModulesManager lModulesManager;
    QString lManagerLastVersion = lModulesManager.fGetModuleLastVersion(lServiceManagerName);
    if(lManagerLastVersion.isEmpty()) {
       qDebug("Module ServiceManager not found!");
       return 1;
    }
 //   NulstarNS::NModuleInfo lServiceManagerInfo = lModulesManager.fModuleInfo(APP_DOMAIN, lServiceManagerName, lManagerLastVersion);

    QSettings lServiceManagerSettings(lConfigFile, QSettings::IniFormat);
    lServiceManagerSettings.beginGroup(lServiceManagerName);
    QString lServiceManagerPort = lServiceManagerSettings.value("CommPort").toString();
    lServiceManagerSettings.endGroup();
    QString lServiceManagerUrl = QHostAddress(QHostAddress::LocalHost).toString();
    lServiceManagerUrl.append(QString(":%1").arg(lServiceManagerPort));
    QWebSocketServer::SslMode lSslMode = QWebSocketServer::SslMode::NonSecureMode;
    if (lSslModeStr.toUShort() == 0)
        lServiceManagerUrl.prepend("ws://");
    if (lSslModeStr.toUShort() == 1) {
        lServiceManagerUrl.prepend("wss://");
        lSslMode = QWebSocketServer::SslMode::SecureMode;
    }

   /* if (lHostAddress.isNull() || lHostAddress.isLoopback())
        lBindAddress = QHostAddress::LocalHost;
    else*/
    lBindAddress = QHostAddress::Any;

    NulstarNS::NMainController lController(lSslMode, static_cast<NulstarNS::NMainController::ELogLevel>(lLogLevel.toUInt()), QUrl(lServiceManagerUrl), lAllowedNetworks, lCommunicationPort.toUShort(), lBindAddress);
    lController.fControlWebServer(QString(), NulstarNS::NMainController::EServiceAction::eStartService); // Start all web sockets servers

    for (const QString& lGroup : lGroups) {
        lSettings.beginGroup(lGroup);
        if (lGroup != "ServiceController") {
            QList<QPair<QString, QString>> lParameters;
            QStringList lKeys(lSettings.childKeys());
            bool lManaged(lSettings.value(lManageParameter, false).toBool());
            if (lManaged) {
                for (const QString& lKey : lKeys) {
                    if (lKey != lManageParameter) {
                        QPair<QString, QString> lParameter;
                        lParameter.first = lKey.toLower();
                        lParameter.second = lSettings.value(lKey).toString();
                        lParameters << lParameter;
                    }
                }
                if (lGroup != "ServiceManager") {
                    QPair<QString, QString> lServiceManagerUrl;
                    lServiceManagerUrl.first = lServiceManagerUrlParameter;
                    lServiceManagerUrl.second = QString("%1:%2").arg(lHostAddress.toString()).arg(lServiceManagerPort);
                    lParameters << lServiceManagerUrl;
                }
                lController.fSetComponent(lGroup, lParameters);
            }
        }
        lSettings.endGroup();
    }
    return lApp.exec();
}
