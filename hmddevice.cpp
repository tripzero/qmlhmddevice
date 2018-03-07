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

#include "hmddevice.h"

#include <QTimer>
#include <QtDebug>
#include <QTransform>
#include <openhmd.h>

HmdDevice::HmdDevice(QObject *parent)
    : QObject(parent), mCtx(ohmd_ctx_create(), ohmd_ctx_destroy),
      mSettings(ohmd_device_settings_create(mCtx.get()), ohmd_device_settings_destroy),
      mUpdateRate(90)
{
    int num_devices = ohmd_ctx_probe(mCtx.get());
    if(num_devices < 0)
    {
        const char * error_msg = ohmd_ctx_get_error(mCtx.get());
        qDebug() << "failed to probe devices: %s\n" << error_msg;
        return;
    }

    // Print device information
    for(int i = 0; i < num_devices; i++){
        qDebug() << "device: " << i;
        qDebug() << "  vendor: " << ohmd_list_gets(mCtx.get(), i, OHMD_VENDOR);
        qDebug() << "  product: " << ohmd_list_gets(mCtx.get(), i, OHMD_PRODUCT);
        qDebug() << "  path: " << ohmd_list_gets(mCtx.get(), i, OHMD_PATH);
    }

    int auto_update = 1;
    ohmd_device_settings_seti(mSettings.get(), OHMD_IDS_AUTOMATIC_UPDATE, &auto_update);

    mDevice = ohmd_list_open_device_s(mCtx.get(), 0, mSettings.get());
    if (!mDevice)
    {
        const char * error_msg = ohmd_ctx_get_error(mCtx.get());
        qCritical() << "Error opening device: " << error_msg;
    }
}

ohmd_device *HmdDevice::ohmdDevice()
{
    return mDevice;
}

uint HmdDevice::updateRate()
{
    return mUpdateRate;
}

void HmdDevice::setUpdateRate(uint rate)
{
    mUpdateRate = rate;
}

void HmdDevice::update()
{
    ohmd_ctx_update(mCtx.get());
}

HmdEye::HmdEye(QObject *parent)
    : QObject(parent), mHmdDevice(nullptr), mCamera(nullptr)
{

}

uint HmdEye::eye()
{
    return (uint)mEye;
}

void HmdEye::setEye(uint e)
{
    if (e != HmdEye::Left || e != HmdEye::Right)
    {
        return;
    }

    mEye = (HmdEye::Eye)e;
}

QObject *HmdEye::hmdDevice()
{
    return mHmdDevice;
}

void HmdEye::setHmdDevice(QObject *device)
{
    bool firstTime = mHmdDevice == nullptr;

    mHmdDevice = static_cast<HmdDevice*>(device);

    if (firstTime)
    {
        QTimer *updateTimer = new QTimer(this);
        QTimer::connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateProperties()));

        updateTimer->setInterval((1.0 / mHmdDevice->updateRate()) * 1000.0);
        updateTimer->start();
    }
}

void HmdEye::setCamera(QObject *camera)
{
    mCamera = static_cast<Qt3DRender::QCamera*>(camera);
}

QMatrix4x4 HmdEye::projectionMatrix()
{
    if (!mHmdDevice)
    {
        qCritical() << "trying to get projection matrix with no device";
        return QMatrix4x4();
    }

    float matrix[16];

    int ret=-1;

    if (mEye == HmdEye::Left)
    {
        ret = ohmd_device_getf(mHmdDevice->ohmdDevice(), OHMD_LEFT_EYE_GL_PROJECTION_MATRIX, matrix);
    }
    else
    {
        ret = ohmd_device_getf(mHmdDevice->ohmdDevice(), OHMD_RIGHT_EYE_GL_PROJECTION_MATRIX, matrix);
    }

    if (ret != OHMD_S_OK)
    {
        qDebug() << "Failed to get projection matrix for eye: " << mEye;
        return QMatrix4x4();
    }

    return QMatrix4x4(matrix);
}

QMatrix4x4 HmdEye::viewMatrix()
{
    if (!mHmdDevice)
    {
        qCritical() << "trying to get projection matrix with no device";
        return QMatrix4x4();
    }

    float matrix[16];

    int ret=-1;

    if (mEye == HmdEye::Left)
    {
        ret = ohmd_device_getf(mHmdDevice->ohmdDevice(), OHMD_LEFT_EYE_GL_MODELVIEW_MATRIX, matrix);
    }
    else
    {
        ret = ohmd_device_getf(mHmdDevice->ohmdDevice(), OHMD_RIGHT_EYE_GL_MODELVIEW_MATRIX, matrix);
    }

    if (ret != OHMD_S_OK)
    {
        qDebug() << "Failed to get projection matrix for eye: " << mEye;
        return QMatrix4x4();
    }

    return QMatrix4x4(matrix);
}

QObject *HmdEye::camera()
{
    return mCamera;
}

void HmdEye::updateProperties()
{
    mHmdDevice->update();
    if (!mCamera)
    {
        return;
    }

    QMatrix4x4 matrix = viewMatrix();
    QMatrix4x4 projection = projectionMatrix();

    mCamera->setProjectionMatrix(projection.transposed());
    mCamera->transform()->setMatrix(matrix.transposed());
}
