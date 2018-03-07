# HmdDevice - QML plugin for HMD device using OpenHMD

## Usage:

HmdDevice is intended to be used with Qt3D.  HmdEye is mapped to a camera
and viewport.

~~~~~~~~~~~~~~{.qml}
import openhmd 1.0

Item {
    HmdDevice {
        id: headset
        updateRate: 60
    }

    HmdEye {
        hmdDevice: headset
        eye: HmdEye.Left
        camera: cameraLeft
    }

    HmdEye {
        hmdDevice: headset
        eye: HmdEye.Right
        camera: cameraRight
    }

    ...
}

~~~~~~~~~~~~~~

The RenderSettings for the scene with two viewports would look something
like this:

~~~~~~~~~~~~~~{.qml}

...
RenderSettings {
    activeFrameGraph: RenderSurfaceSelector {
        Viewport {
            normalizedRect: Qt.rect(0, 0, 1, 1)
        }
        ClearBuffers {
            buffers: ClearBuffers.ColorDepthBuffer
            clearColor: Qt.rgba(0.0, 0.0, 0.0, 0.0)
        }
        Viewport {
            id: left
            nomralizedRect: Qt.rect(0, 0, 0.5, 1)
            CameraSelector {
                camera: Camera {
                    id: cameraLeft
                }
            }
        }
        Viewport {
            id: right
            nomralizedRect: Qt.rect(0.5, 0, 0.5, 1)
            CameraSelector {
                camera: Camera {
                    id: cameraRight
                }
            }
        }
    }
}
~~~~~~~~~~~~~~

