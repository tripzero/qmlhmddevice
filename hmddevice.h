/*
    Copyright (C) 2018 Kevron Rees

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef HMDDEVICE_H
#define HMDDEVICE_H

#include <QObject>
#include <QQuaternion>
#include <QMatrix4x4>
#include <Qt3DRender/QCamera>

#include <memory>

#include <openhmd.h>

class HmdDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint updateRate READ updateRate WRITE setUpdateRate NOTIFY updateRateChanged)

public:
    HmdDevice(QObject *parent = nullptr);
    ohmd_device *ohmdDevice();

    ///rate to update in Hz
    uint updateRate();
    void setUpdateRate(uint rate);

    void update();

signals:
    void updateRateChanged(uint rate);

private:
    std::unique_ptr<ohmd_context, void(*)(ohmd_context *)> mCtx;
    std::unique_ptr<ohmd_device_settings, void(*)(ohmd_device_settings *)> mSettings;
    ohmd_device *mDevice;
    uint mUpdateRate;
};

class HmdEye : public QObject
{
    Q_OBJECT
    Q_ENUMS(Eye)
    Q_PROPERTY(uint eye READ eye WRITE setEye NOTIFY eyeChanged)
    Q_PROPERTY(QObject *hmdDevice READ hmdDevice WRITE setHmdDevice NOTIFY hmdDeviceChanged)
    Q_PROPERTY(QObject *camera READ camera WRITE setCamera NOTIFY cameraChanged)

public:
    enum Eye { Right=0, Left=1};

    explicit HmdEye(QObject *parent = nullptr);

    uint eye();
    void setEye(uint);
    QObject *hmdDevice();
    void setHmdDevice(QObject *device);
    void setCamera(QObject *camera);

    QMatrix4x4 projectionMatrix();

    QObject *camera();

    QMatrix4x4 viewMatrix();

signals:
    void eyeChanged(uint e);
    void hmdDeviceChanged();
    void cameraChanged();

private slots:
    void updateProperties();

private:
    Eye mEye;
    HmdDevice *mHmdDevice;
    Qt3DRender::QCamera *mCamera;
    QMatrix4x4 mProjectionMatrix;
    QQuaternion mRotation;
};

#endif // HMDDEVICE_H
