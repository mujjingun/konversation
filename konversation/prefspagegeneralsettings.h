/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  prefspagegeneralsettings.h  -  Provides a user interface to customize general settings
  begin:     Fre Nov 15 2002
  copyright: (C) 2002 by Dario Abatianni
  email:     eisfuchs@tigress.com

  $Id$
*/


#ifndef PREFSPAGEGENERALSETTINGS_H
#define PREFSPAGEGENERALSETTINGS_H

#include <prefspage.h>

/*
  @author Dario Abatianni
*/

class PrefsPageGeneralSettings : public PrefsPage
{
  Q_OBJECT

  public:
    PrefsPageGeneralSettings(QFrame* newParent,Preferences* newPreferences);
    ~PrefsPageGeneralSettings();

  protected slots:
    void commandCharChanged(const QString& newChar);
    void suffixStartChanged(const QString& newSuffix);
    void suffixMiddleChanged(const QString& newSuffix);
    void channelActionChanged(const QString& newAction);
    void notifyActionChanged(const QString& newAction);
    void autoReconnectChanged(int state);
    void autoRejoinChanged(int state);
    void blinkingTabsChanged(int state);
    void bringToFrontChanged(int state);
    void fixedMOTDChanged(int state);
    void beepChanged(int state);
    void rawLogChanged(int state);
};

#endif
