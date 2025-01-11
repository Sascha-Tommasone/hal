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

#include "gui/searchbar/search_proxy_model.h"

#include <QSortFilterProxyModel>

namespace hal
{
    /**
     * @ingroup utility_widgets-context
     * @brief A proxy model to filter the ContextTableModel by a given string.
     */
    class PinProxyModel : public SearchProxyModel
    {
        Q_OBJECT

    public:
        /**
         * The constructor.
         *
         * @param parent - The widget's parent.
         */
        PinProxyModel(QObject* parent = nullptr);

        /**
         * Determines if the index specified by the row and its parent should be displayed
         * given the before set filter.
         *
         * @param source_row - The index's row.
         * @param source_parent - The index's parent.
         *
         * @return True if it should be displayed, False otherwise.
         */
        bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    public Q_SLOTS:

        /**
         * Sets the proxy model's filter to the given string
         * and sets the searchOptions.
         *
         * @param filter_text - The new filter string.
         * @param SearchOptions_bitmask - SearchOptions.
         */
        void startSearch(QString text, int options) override;



    };
}    // namespace hal
