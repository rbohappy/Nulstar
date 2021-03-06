#ifndef NMESSAGENEGOTIATECONNECTIONRESPONSE_H
#define NMESSAGENEGOTIATECONNECTIONRESPONSE_H

#include <QObject>
#include <QString>
#include <QVariantMap>

#include "NMessage.h"
#include "NNetwork.h"

namespace NulstarNS {
  const QString cNegotiationStatusFieldName("NegotiationStatus");
  const QString cNegotiationCommentFieldName("NegotiationComment");
  const int cNegotiationStatusSuccess = 1;
  const int cNegotiationStatusFailure = 0;

  class NETWORKSHARED_EXPORT NMessageNegotiateConnectionResponse : public NMessage {
    Q_OBJECT

    public:
      enum class ENegotiationStatus { eNegotiationError = 0, eNegotiationSuccessful = 1 };

      explicit NMessageNegotiateConnectionResponse(const QString& lConnectionName, const QString& lMessageID = QString(), ENegotiationStatus lNegotiationStatus = ENegotiationStatus::eNegotiationSuccessful, const QString& lNegotiationComment = QString(), QObject* rParent = nullptr);
      ~NMessageNegotiateConnectionResponse() override {}

      ENegotiationStatus fNegotiationStatus() const { return mNegotiationStatus; }
      QString fNegotiationComment() const { return mNegotiationComment; }
      static bool fValidateMessageObject(const QJsonObject& lMessageObject);

    protected:
      QVariantMap fMessageData() const override;
      QString fMessageType() const override { return cTypeNegotiateConnectionResponse; }

    private:
      ENegotiationStatus mNegotiationStatus;
      QString mNegotiationComment;
  };
}

#endif // NMESSAGENEGOTIATECONNECTIONRESPONSE_H
