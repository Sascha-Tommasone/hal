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
#include "qsortfilterproxymodel.h"
#include "gui/searchbar/searchoptions.h"

namespace hal
{
    class SearchProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        SearchProxyModel(QObject* parent = nullptr);

        /**
         * @return List of QString matching the corresponding column header.
         */
        virtual QList<QString> getColumnNames();

    public Q_SLOTS:
        virtual void startSearch(QString text, int options) = 0;

    protected:
        /**
         * @brief Check if a string matches the SearchOptions.
         *
         * This function checks whether a given `stringToCheck` matches the search criteria
         * specified by `searchString` based on search options.
         *
         * @param searchString The search criteria to match against `stringToCheck`.
         * @param stringToCheck The string to be checked for a match.
         * @return True if `stringToCheck` matches the search criteria, otherwise false.
         */
        bool isMatching(const QString searchString, const QString stringToCheck) const;

        /**
         * @brief Should be called inside filterAcceptsRow function and returns true if the source_row, source_parent matches given SearchOptions
         *
         * This function checks whether a given `source_row` should be shown
         * based on given `search options` .
         *
         * @param sourceRow The row index in the source model to be checked.
         * @param sourceParent The QModelIndex representing the parent of the source model.
         * @param startIndex The index of the first column to start checking.
         * @param endIndex The index of the last column to stop checking.
         * @param offset The offset to apply to the column indices based on search options if specific columns are checked.
         *
         * @return True if the row contains a matching entry, otherwise false.
         */
        bool checkRow(int sourceRow, const QModelIndex& sourceParent, int startIndex, int endIndex, int offset = 0) const;

        /**
         * Convenience method for tree model
         */
        virtual bool checkRowRecursion(int sourceRow, const QModelIndex& sourceParent, int startIndex, int endIndex, int offset = 0) const;
        SearchOptions mSearchOptions;
        QString mSearchString;
    };
}

