// MIT License
// 
// Copyright (c) 2019 Ruhr University Bochum, Chair for Embedded Security. All Rights reserved.
// Copyright (c) 2019 Marc Fyrbiak, Sebastian Wallat, Max Hoffmann ("ORIGINAL AUTHORS"). All rights reserved.
// Copyright (c) 2021 Max Planck Institute for Security and Privacy. All Rights reserved.
// Copyright (c) 2021 Jörn Langheinrich, Julian Speith, Nils Albartus, René Walendy, Simon Klix ("ORIGINAL AUTHORS"). All Rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "gui/searchbar/searchoptions.h"
#include <QFrame>

class QLabel;
class QLineEdit;
class QHBoxLayout;
class QPushButton;
class QToolButton;

namespace hal
{
    /**
     * @ingroup gui
     * @brief A QFrame with a QLineEdit that can be used to input a substring to search for
     *
     * The Searchbar consists of a QLineEdit which can be filled with a substring to search for. If the text within
     * the searchbar changes, a signal textEdited is emitted. Other objects that use the input of the Searchbar may
     * want to connect to this signal to update their respective view.
     */
    class Searchbar : public QFrame
    {
        Q_OBJECT
        Q_PROPERTY(QString searchIcon READ searchIcon WRITE setSearchIcon)
        Q_PROPERTY(QString searchIconStyle READ searchIconStyle WRITE setSearchIconStyle)
        Q_PROPERTY(QString clearIcon READ clearIcon WRITE setClearIcon)
        Q_PROPERTY(QString clearIconStyle READ clearIconStyle WRITE setClearIconStyle)
        Q_PROPERTY(QString optionDialogIcon READ optionDialogIcon WRITE setOptionDialogIcon)
        Q_PROPERTY(QString optionDialogIconStyle READ optionDialogIconStyle WRITE setOptionDialogIconStyle)

    public:
        /**
         * Constructor.
         *
         * @param parent - The parent widget
         */
        Searchbar(QWidget* parent = nullptr);

        /// @name Q_PROPERTY READ Functions
        ///@{
        QString searchIcon() const;
        QString searchIconStyle() const;
        QString clearIcon() const;
        QString clearIconStyle() const;
        QString optionDialogIcon() const;
        QString optionDialogIconStyle() const;
        ///@}

        /// @name Q_PROPERTY WRITE Functions
        ///@{
        void setSearchIcon(const QString& icon);
        void setSearchIconStyle(const QString& style);
        void setClearIcon(const QString& icon);
        void setClearIconStyle(const QString& style);
        void setColumnNames(QList<QString> list);
        void setOptionDialogIcon(const QString& icon);
        void setOptionDialogIconStyle(const QString& style);
        ///@}

        /**
         * Sets the placeholder text of the QLineEdit of this Searchbar. The placeholder text is displayed if the
         * QLineEdit is empty.
         *
         * @param text - The new placeholder text
         */
        void setPlaceholderText(const QString& text);

        /**
         * Empties the Searchbar's QLineEdit.
         */
        void clear();

        /**
         * Emits textEdited with empty string when hide() is called
         */
        void hideEvent(QHideEvent *) override;

        /**
         * Emits textEdited with getCurrentText() when show() is called
         */
        void showEvent(QShowEvent *) override;

        /**
         * Gets the current Searchbar input, i.e. the current text in the Searchbar's QLineEdit.
         *
         * @return the current search string
         */
        QString getCurrentText();

        /**
         * Gets the current Searchbar input with regex modifier.
         *
         * @return the current search string
         */
        QString getCurrentTextWithFlags();

        /**
         * Adds flags ("Exact Match" or "Case Sensitive") as regex modifiers to text.
         *
         * @return text with regex modifiers as QString
         */
        QString addFlags(const QString& text);

        /**
         * Reinitializes the appearance of the Searchbar.
         */
        void repolish();

        SearchOptions getSearchOptions() const;
        /**
         * @return mExactMatch->isChecked()
         */
        // bool exactMatchChecked();

        /**
         * @return mCaseSensitive->isChecked()
         */
        // bool caseSensitiveChecked();

        /**
         * A filter is applied when the searchbar contains text or (at least) one of the flags
         * ("Exact Match" or "Case Sensitive") is checked.
         *
         * @return if a filter is applied
         */
        bool filterApplied();

        void setEmitTextWithFlags(bool);

        bool getEmitTextWithFlags();

        bool eventFilter(QObject *object, QEvent *event) override;


    Q_SIGNALS:
        /**
         * Q_SIGNAL that is emitted whenever the Proxy should be updated or a new search should be issued
         * @param text - The new search string
         * @param searchOptions - Search options encoded as int (use  SearchOptions(int) constructor to get settings)
         */
        void triggerNewSearch(const QString& text, int searchOptions);

        /**
         * Q_SIGNAL that is emitted whenever the Return/Enter key has been pressed.
         */
        void returnPressed();

        // testing, might be removed
        void searchIconClicked();

    public Q_SLOTS:
        /**
         * Handle textEdited signal, might emit triggerNewSearch with respect to mEmitTextWithFlags.
         */
        void handleTextEdited();

        /**
         * Handles Return/Enter key pressed. Emits the signal returnPressed.
         */
        void handleReturnPressed();

        /**
         * Handles "Clear"-Button clicks.
         */
        void handleClearClicked();

        void handleSearchOptionsDialog();

        void updateSearchHistory(QString entry);

    private:
        QHBoxLayout* mLayout;

        QLabel* mSearchIconLabel;
        QLineEdit* mLineEdit;
        QLabel* mClearIconLabel;

        QToolButton* mDownButton;
        QToolButton* mUpButton;
        //QToolButton* mCaseSensitiveButton;
        QToolButton* mSearchOptionsButton;
        QToolButton* mClearButton;

        QString mSearchIcon;
        QString mSearchIconStyle;
        QString mClearIcon;
        QString mClearIconStyle;
        QString mOptionDialogIcon;
        QString mOptionDialogIconStyle;

        QList<QString> mColumnNames;


        // One can decide wether to receive the text (emitted by textEdited) with or without regex modifier
        // If set to false, one has to manually implement 'Exact Match'/'Case Sensitive' functionality
        bool mEmitTextWithFlags = true;

        SearchOptions* mCurrentOptions;
        bool mIncrementalSearch = true;
        int mMinCharsToStartIncSearch = 3;

        QStringList mSearchHistory;
    };
}
