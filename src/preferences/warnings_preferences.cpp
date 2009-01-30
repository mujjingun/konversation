/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2006 Dario Abatianni <eisfuchs@tigress.com>
  Copyright (C) 2006 John Tapsell <johnflux@gmail.com>
  Copyright (C) 2006-2008 Eike Hein <hein@kde.org>
*/


#include "warnings_preferences.h"

#include <q3listview.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <k3listview.h>
#include <kglobal.h>


Warnings_Config::Warnings_Config( QWidget* parent, const char* name, Qt::WFlags fl )
    : Warnings_ConfigUI( parent, name, fl )
{
  dialogListView->setSorting(1);
  loadSettings();
  connect(dialogListView, SIGNAL(clicked(Q3ListViewItem *)), this, SIGNAL(modified()));
}

Warnings_Config::~Warnings_Config()
{
}

void Warnings_Config::restorePageToDefaults()
{

  Q3CheckListItem* item=static_cast<Q3CheckListItem*>(dialogListView->itemAtIndex(0));
  bool changed=false;
  while(item)
  {
    if(!item->isOn()) {
      item->setOn(true);
      changed=true;
    }
    item=static_cast<Q3CheckListItem*>(item->itemBelow());
  }
  if(changed) {
    emit modified();
  }
}

void Warnings_Config::saveSettings()
{
  KSharedConfigPtr config = KGlobal::config();
  KConfigGroup grp = config->group("Notification Messages");

  // prepare list
  QString warningsChecked;

  Q3CheckListItem* item=static_cast<Q3CheckListItem*>(dialogListView->itemAtIndex(0));
  int i = 0;
  while(item)
  {
    // save state of this item in hasChanged() list
    warningsChecked+=item->isOn();

    if (item->text(2) == "LargePaste" || item->text(2) == "Invitation")
    {
        if (item->isOn())
        {
            grp.writeEntry(item->text(2), 1);
        }
        else
        {
            QString state = grp.readEntry(item->text(2));

            if (!state.isEmpty() && (state == "yes" || state == "no"))
                grp.writeEntry(item->text(2), state);
            else
                grp.writeEntry(item->text(2), "yes");
        }
    }
    else
    {
        grp.writeEntry(item->text(2),item->isOn() ? "1" : "0");
    }

    item=static_cast<Q3CheckListItem*>(item->itemBelow());
    ++i;
  }

  // remember checkbox state for hasChanged()
  m_oldWarningsChecked=warningsChecked;
}

void Warnings_Config::loadSettings()
{
  QStringList dialogDefinitions;
  QString flagNames = "Invitation,SaveLogfileNote,ClearLogfileQuestion,CloseQueryAfterIgnore,ReconnectWithDifferentServer,ReuseExistingConnection,QuitServerTab,QuitChannelTab,QuitQueryTab,ChannelListNoServerSelected,HideMenuBarWarning,ChannelListWarning,LargePaste,systemtrayquitKonversation,IgnoreNick,UnignoreNick,QuitWithActiveDccTransfers";
  dialogDefinitions.append(i18n("Automatically join channel on invite"));
  dialogDefinitions.append(i18n("Notice that saving logfiles will save whole file"));
  dialogDefinitions.append(i18n("Ask before deleting logfile contents"));
  dialogDefinitions.append(i18n("Ask about closing queries after ignoring the nickname"));
  dialogDefinitions.append(i18n("Ask before switching a connection to a network to a different server"));
  dialogDefinitions.append(i18n("Ask before creating another connection to the same network or server"));
  dialogDefinitions.append(i18n("Close server tab"));
  dialogDefinitions.append(i18n("Close channel tab"));
  dialogDefinitions.append(i18n("Close query tab"));
  dialogDefinitions.append(i18n("The channel list can only be opened from server-aware tabs"));
  dialogDefinitions.append(i18n("Warning on hiding the main window menu"));
  dialogDefinitions.append(i18n("Warning on high traffic with channel list"));
  dialogDefinitions.append(i18n("Warning on pasting large portions of text"));
  dialogDefinitions.append(i18n("Warning on quitting Konversation"));
  dialogDefinitions.append(i18n("Ignore"));
  dialogDefinitions.append(i18n("Unignore"));
  dialogDefinitions.append(i18n("Warn before quitting with active DCC file transfers"));
  Q3CheckListItem *item;
  dialogListView->clear();

  KSharedConfigPtr config = KGlobal::config();
  KConfigGroup grp =  config->group("Notification Messages");
  QString flagName;
  for(unsigned int i=0; i<dialogDefinitions.count() ;i++)
  {
    item=new Q3CheckListItem(dialogListView,dialogDefinitions[i],Q3CheckListItem::CheckBox);
    item->setText(1,dialogDefinitions[i]);
    flagName = flagNames.section(",",i,i);
    item->setText(2,flagName);

    if (flagName == "LargePaste" || flagName == "Invitation")
    {
        QString state = grp.readEntry(flagName);

        if (state == "yes" || state == "no")
            item->setOn(false);
        else
            item->setOn(true);
    }
    else
    {
        item->setOn(grp.readEntry(flagName,true));
    }
  }
  // remember checkbox state for hasChanged()
  m_oldWarningsChecked=currentWarningsChecked();
}

// get a list of checked/unchecked items for hasChanged()
QString Warnings_Config::currentWarningsChecked()
{
  // prepare list
  QString newList;

  // get first checklist item
  Q3ListViewItem* item=dialogListView->firstChild();
  while(item)
  {
    // save state of this item in hasChanged() list
    newList+=(static_cast<Q3CheckListItem*>(item)->isOn()) ? "1" : "0";
    item=item->itemBelow();
  }
  // return list
  return newList;
}

bool Warnings_Config::hasChanged()
{
  return(m_oldWarningsChecked!=currentWarningsChecked());
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Warnings_Config::languageChange()
{
  loadSettings();
}

#include "warnings_preferences.moc"
