/***************************************************************************
 *   Copyright (C) 2011 by Nicolae Ghimbovschi                             *
 *     nicolae.ghimbovschi@gmail.com                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 3 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "taskbarbutton.h"

#ifdef Q_OS_WIN32
#include "win7_include.h"

namespace QW7 {
    struct TaskbarButton::PrivateData {
        ITaskbarList3* m_handler;

        PrivateData() {
            m_handler = NULL;

            HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3,
                                          reinterpret_cast<void**> (&(m_handler)));

            if (SUCCEEDED(hr)){

                hr = m_handler->HrInit();

                if (FAILED(hr))
                {
                    m_handler->Release();
                    m_handler = NULL;
                }
            }
        }

        ITaskbarList3* TBHandler() { return m_handler;}

        ~PrivateData() {
            if (m_handler) {
                m_handler->Release();
            }
        }
    };

    TaskbarButton::TaskbarButton(QObject *parent) : QObject(parent) {
        m_private = NULL;
        m_widget_id = dynamic_cast<QWidget*>(parent)->winId();
    }

    void TaskbarButton::Init() {
        if (!m_private) {
            m_private = new PrivateData();
        }
    }

    long TaskbarButton::SetOverlayIcon(const QIcon& icon, QString description) {
        if (m_private) {

            HICON overlay_icon = icon.pixmap(48).toWinHICON();
            if (overlay_icon) {
                long result = m_private->TBHandler()->SetOverlayIcon(m_widget_id, overlay_icon, description.toStdWString().c_str());

                DestroyIcon(overlay_icon);

                return result;
            }
        }

        return -1;
    }

    long TaskbarButton::SetState(TB_STATE state) {
        if (m_private) {
            return m_private->TBHandler()->SetProgressState(m_widget_id, (TBPFLAG)state);
        }

        return -1;
    }

    long TaskbarButton::SetProgresValue(unsigned long long done, unsigned long long total) {
        if (m_private) {
            return m_private->TBHandler()->SetProgressValue(m_widget_id, done, total);
        }

        return -1;
    }


    TaskbarButton::~TaskbarButton() {
        if (m_private) {
            delete m_private;
        }
    }

}

#endif //Q_OS_WIN32