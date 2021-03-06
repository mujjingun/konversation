/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2006 Dario Abatianni <eisfuchs@tigress.com>
  Copyright (C) 2006 John Tapsell <johnflux@gmail.com>
*/

#ifndef NICKLISTBEHAVIOR_CONFIG_H
#define NICKLISTBEHAVIOR_CONFIG_H

#include "settingspage.h"
#include "ui_nicklistbehavior_configui.h"


class NicklistBehavior_Config : public QWidget, public KonviSettingsPage, private Ui::NicklistBehavior_ConfigUI
{
    Q_OBJECT

    public:
        explicit NicklistBehavior_Config(QWidget *parent = 0, const char *name = 0);
        ~NicklistBehavior_Config();

        void saveSettings() Q_DECL_OVERRIDE;
        void loadSettings() Q_DECL_OVERRIDE;
        void restorePageToDefaults() Q_DECL_OVERRIDE;

        bool hasChanged() Q_DECL_OVERRIDE;

    private:
        void setNickList(const QString &sortingOrder);
        QString currentSortingOrder();

        QString m_oldSortingOrder;

    Q_SIGNALS:
        void modified();
};

#endif
