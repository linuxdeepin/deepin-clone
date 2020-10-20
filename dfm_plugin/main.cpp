/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <dfmfilecontrollerplugin.h>
#include <dfmeventdispatcher.h>
#include <dde-file-manager/dfileservices.h>
#include <dfmabstracteventhandler.h>

#include <QWidget>

#include "dimfilecontroller.h"
#include "../app/src/dglobal.h"

bool Global::isOverride = true;
bool Global::disableMD5CheckForDimFile = false;
bool Global::disableLoopDevice = true;
bool Global::fixBoot = false;
bool Global::isTUIMode = false;

int Global::bufferSize = 1024 * 1024;
int Global::compressionLevel = 0;
int Global::debugLevel = 1;

DFM_USE_NAMESPACE

class DIMFileEventFilter : public DFMAbstractEventHandler
{
public:
    bool fmEventFilter(const QSharedPointer<DFMEvent> &event, DFMAbstractEventHandler *target = 0, QVariant *resultData = 0) override
    {
        Q_UNUSED(target)
        Q_UNUSED(resultData)

        if (event->type() != DFMEvent::OpenFile) {
            return false;
        }

        const DUrl &url = dfmevent_cast<DFMOpenFileEvent>(*event.data()).url();

        if (!url.isLocalFile())
            return false;

        const DAbstractFileInfoPointer &info = DFileService::instance()->createFileInfo(0, url);

        if (info->mimeTypeName() == "application/x-deepinclone-dim") {
            const QWidget *widget = qobject_cast<const QWidget*>(event->sender());

            if (!widget)
                return false;

            DFMEventDispatcher::instance()->processEvent<DFMChangeCurrentUrlEvent>(event->sender(), DUrl("dim://" + url.toLocalFile()), widget->window());

            return true;
        }

        return false;
    }
};

class DIMFilePlugin : public DFMFileControllerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DFMFileControllerFactoryInterface_iid FILE "dfm-plugin-dim-file.json")

public:
    explicit DIMFilePlugin(QObject *parent = 0)
        : DFMFileControllerPlugin(parent) {
        DFMEventDispatcher::instance()->installEventFilter(new DIMFileEventFilter());
    }

    virtual DAbstractFileController *create(const QString &key) override {
        Q_UNUSED(key)

        return new DIMFileController();
    }
};

#include "main.moc"
