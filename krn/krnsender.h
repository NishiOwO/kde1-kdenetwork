#ifndef KRN_SENDER_H
#define KRN_SENDER_H

#include <qobject.h>

#include "kmsender.h"
#include "NNTP.h"

class KRNSender: public QObject,public KMSender
{
    Q_OBJECT
public:
    void setNNTP(NNTP *);
    bool send(KMMessage *aMsg, short sendNow=-1);
private:
    bool sendNow(KMMessage *aMsg);
    bool queue(KMMessage *aMsg);
    NNTP *server;
};

#endif