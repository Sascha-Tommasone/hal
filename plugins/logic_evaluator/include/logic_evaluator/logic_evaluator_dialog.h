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

#include <QDialog>
#include <QList>
#include <vector>
#include <dlfcn.h>
#include <QTemporaryFile>
#include <QCheckBox>
#include <QMenuBar>
#include <QAction>
#include "hal_core/netlist/boolean_function.h"

namespace hal {
    class SimulationInput;

    class Gate;

    class Net;

    class LogicEvaluatorPingroup;

    class LogicEvaluatorTruthtableModel;

    class LogicEvaluatorDialog : public QDialog
    {
        Q_OBJECT

        struct SharedLibHandle
        {
            QString fnSharedLib;
            void* handle;
            int (*get)(int);
            void (*set)(int,int);
            void (*calc)(void);
            void close();
            SharedLibHandle() : handle(nullptr), get(nullptr), set(nullptr), calc(nullptr) {;}
            ~SharedLibHandle() { close(); };
        };

        std::vector<Gate*> mGates;
        SimulationInput* mSimulationInput;
        QList<const Gate*> mEvaluationOrder;
        QList<LogicEvaluatorPingroup*> mInputs;
        QList<LogicEvaluatorPingroup*> mOutputs;
        SharedLibHandle mSharedLib;
        QHash<const Net*, int> mExternalArrayIndex;
        QHash<const Net*, BooleanFunction::Value> mSignals;
        QMenuBar* mMenuBar;
        QAction* mActionCompile;
        QAction* mActionIndicate;
        LogicEvaluatorTruthtableModel* mTruthtable;

        void calculateEvaluationOrder();
        void recalcCompiled();
        void recalcInterpreted();
        void visualizeResultsInNetlist();
        void omitNetlistVisualization();
    public Q_SLOTS:
        void recalc();
    private Q_SLOTS:
        void handleCompiledToggled(bool checked);
        void handleIndicateToggled(bool checked);
        void handleRelaunchTriggered();
        void handleTruthtableTriggered();
    public:
        LogicEvaluatorDialog(const std::vector<Gate*>& gates, bool skipCompile, QWidget* parent = nullptr);
        ~LogicEvaluatorDialog();
        bool compile();
    };
}
