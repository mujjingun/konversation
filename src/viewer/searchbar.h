/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

/*
  Copyright (C) 2005 Renchi Raju <renchi@pooh.tam.uiuc.edu>
  Copyright (C) 2006, 2016 Peter Simonsson <peter.simonsson@gmail.com>
*/

#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include "ui_searchbarbase.h"

#include <QIcon>
#include <QTextDocument>

class QShortcut;

class QMenu;

class SearchBar : public QWidget, private Ui::SearchBarBase
{
    Q_OBJECT

    public:
        explicit SearchBar(QWidget* parent);
        ~SearchBar();

        void setHasMatch(bool value);

        QString pattern() const;

        QTextDocument::FindFlags flags() const;

        bool fromCursor() const;

        bool eventFilter(QObject* object, QEvent* e) Q_DECL_OVERRIDE;

    protected:
        void showEvent(QShowEvent* e) Q_DECL_OVERRIDE;
        void hideEvent(QHideEvent* e) Q_DECL_OVERRIDE;
        bool focusedChild();

    private Q_SLOTS:
        void slotTextChanged();
        void slotFind();
        void slotFindNext();
        void slotFindPrevious();

        void toggleMatchCase(bool value);
        void toggleWholeWords(bool value);

    Q_SIGNALS:
        void signalSearchChanged(const QString& pattern);
        void signalSearchNext();
        void signalSearchPrevious();
        void hidden();

    private:
        QTimer* m_timer;

        QMenu* m_optionsMenu;
        QIcon m_goUpSearch;
        QIcon m_goDownSearch;

        QTextDocument::FindFlags m_flags;
        bool m_fromCursor;

        QShortcut* m_closeShortcut;
};

#endif                                            /* SEARCHBAR_H */
