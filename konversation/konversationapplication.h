/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  konversationapplication.h  -  The main application
  begin:     Mon Jan 28 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com
*/

#ifndef KONVERSATIONAPPLICATION_H
#define KONVERSATIONAPPLICATION_H

#include <kapp.h>

#include "preferences.h"
#include "ircevent.h"
#include "osd.h"

class QCString;

class KonversationMainWindow;
class KonvDCOP;
class Server;
class PrefsDialog;

/*
  @author Dario Abatianni
*/

class KonversationApplication : public KApplication
{
  Q_OBJECT

  public:
    static Preferences preferences;

    // URL-Catcher
    void storeUrl(const QString& who,const QString& url);
    const QStringList& getUrlList();

    // DCOP: Returns a list of signals we should emit
    QPtrList<IRCEvent> retrieveHooks(EVENT_TYPE type);

    KonversationApplication();
    ~KonversationApplication();

    void syncPrefs();
    Server* getServerByName(const QString& name);
    OSDWidget* osd;

  signals:
    void catchUrl(const QString& who,const QString& url);

  public slots:
    void connectToServer(int number);
    void connectToAnotherServer(int number);
    void readOptions();
    void saveOptions(bool updateGUI=true);
    void quitKonversation();

    void closePrefsDialog();

    void deleteUrl(const QString& who,const QString& url);
    void clearUrlList();

    bool emitDCOPSig(const QString& appId, const QString& objId, const QString& signal, QByteArray& data);

  protected slots:
    void openPrefsDialog();
    void openPrefsDialog(Preferences::Pages page);
    void removeServer(Server* server);
    void dcopSay(const QString& server,const QString& target,const QString& command);
    void dcopInfo(const QString& string);

  protected:
    QPtrList<Server> serverList;
    QStringList urlList;
    PrefsDialog* prefsDialog;
    KonvDCOP* dcopObject;
    KonversationMainWindow* mainWindow;
};

#endif
