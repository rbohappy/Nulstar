#include <QDateTime>

#include "NMessage.h"

namespace NulstarNS {
  qint64 NMessage::mMessageIDPrefix = 0;
  qint64 NMessage::mMessageIDSuffix = 0;

  NMessage::NMessage(const QString& lConnectionName, const QString& lMessageID, QObject *rParent)
          : QObject(rParent), mConnectionName(lConnectionName), mMessageID(lMessageID) {
    mStatus = EMessageStatus::eAwaitingDelivery;
    mTimeStamp = QDateTime::currentDateTime().currentMSecsSinceEpoch();
    mTimeZone = QDateTime::currentDateTime().offsetFromUtc() / 3600;

    if(mMessageID.isEmpty()) {
      if(mMessageIDPrefix == 0)
        mMessageIDPrefix = mTimeStamp; // static value only executes firts message of the session
      ++mMessageIDSuffix;
      mMessageID = QString("%1-%2").arg(QString::number(mMessageIDPrefix)).arg(QString::number(mMessageIDSuffix));
    }
  }

  QString NMessage::fToJsonString(QJsonDocument::JsonFormat lFormat) const {
    QVariantMap lMessage;
    lMessage.insert(cMessageIDFieldName, mMessageID);
    lMessage.insert(cTimeStampFieldName, mTimeStamp);
    lMessage.insert(cTimeZoneFieldName, mTimeZone);
    lMessage.insert(cMessageTypeFieldName, fMessageType());
    lMessage.insert(cMessageDataFieldName, fMessageData());
    return QString(QJsonDocument::fromVariant(lMessage).toJson(lFormat));
  }

  bool NMessage::fValidateMessageObject(const QJsonObject& lMessageObject) {
    if(!lMessageObject.contains(cMessageIDFieldName)) {
      qDebug("Message received without 'ID' field!");
      return false;
    }
    if(!lMessageObject.contains(cTimeStampFieldName)) {
      qDebug("Message received without 'Timestamp' field!");
      return false;
    }
    if(!lMessageObject.contains(cTimeZoneFieldName)) {
      qDebug("Message received without 'TimeZone' field!");
      return false;
    }
    if(!lMessageObject.contains(cMessageTypeFieldName)) {
      qDebug("Message received without '' field!");
      return false;
    }
    if(!lMessageObject.contains(cMessageDataFieldName)) {
      qDebug("Message received without 'Data' field!");
      return false;
    }
    return true;
  }
}
