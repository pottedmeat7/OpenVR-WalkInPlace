
#pragma once

#include <openvr.h>
#include <QtCore/QtCore>
// because of incompatibilities with QtOpenGL and GLEW we need to cherry pick includes
#include <QVector2D>
#include <QMatrix4x4>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QObject>
#include <QSettings>
#include <QQmlEngine>
#include <QtGui/QOpenGLContext>
#include <QtWidgets/QGraphicsScene>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QtWidgets/QWidget>
#include <QQuickItem>
#include <QQuickRenderControl>
#include <QtCore/QObject>
#include <memory>
#include "logging.h"

#include "tabcontrollers/WalkInPlaceTabController.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
static const std::string slash="\\";
#else
static const std::string slash="/";
#endif

// application namespace
namespace walkinplace {

class OverlayController : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool desktopMode READ isDesktopMode)

public:
	static constexpr const char* applicationKey = "pottedmeat7.VRWalkInPlace";
	static constexpr const char* applicationName = "OpenVR-WalkInPlace";
	static constexpr const char* applicationVersionString = "v4.3";

private:
	vr::VROverlayHandle_t m_ulOverlayHandle = vr::k_ulOverlayHandleInvalid;
	vr::VROverlayHandle_t m_ulOverlayThumbnailHandle = vr::k_ulOverlayHandleInvalid;

	std::unique_ptr<QGraphicsScene> m_pScene;
	std::unique_ptr<QOpenGLFramebufferObject> m_pFbo;
	std::unique_ptr<QOpenGLContext> m_pOpenGLContext;
	std::unique_ptr<QOffscreenSurface> m_pOffscreenSurface;

	std::unique_ptr<QTimer> m_pPumpEventsTimer;

	QWidget *m_pWidget;

	bool dashboardVisible = false;

	QPointF m_ptLastMouse;
	Qt::MouseButtons m_lastMouseButtons = 0;

	bool desktopMode;
	bool directMode;

	QUrl m_runtimePathUrl;

public: // I know it's an ugly hack to make them public to enable external access, but I am too lazy to implement getters.
	WalkInPlaceTabController walkInPlaceTabController;

private:
    OverlayController(bool desktopMode, bool directMode) : QObject(), desktopMode(desktopMode), directMode(directMode) {}

public:
	virtual ~OverlayController();

	void Init(QQmlEngine* qmlEngine);
	void Shutdown();

	bool isDashboardVisible() {
		return dashboardVisible;
	}

	void SetWidget(QWidget* quickItem);

	bool isDesktopMode() { return desktopMode; };

	Q_INVOKABLE QString getVersionString();
	Q_INVOKABLE QUrl getVRRuntimePathUrl();

	bool isDirectMode();

	const vr::VROverlayHandle_t& overlayHandle();
	const vr::VROverlayHandle_t& overlayThumbnailHandle();

public slots:
	void OnSceneChanged( const QList<QRectF>& );
	void OnTimeoutPumpEvents();

	void showKeyboard(QString existingText, unsigned long userValue = 0);

signals:
	void keyBoardInputSignal(QString input, unsigned long userValue = 0);

private:
	static QSettings* _appSettings;
	static std::unique_ptr<OverlayController> singleton;

public:
	static OverlayController* getInstance() {
		return singleton.get();
	}

	static OverlayController* createInstance(bool desktopMode, bool directMode) {
		singleton.reset(new walkinplace::OverlayController(desktopMode, directMode));
		return singleton.get();
	}

	static QSettings* appSettings() { return _appSettings; }

	static void setAppSettings(QSettings* settings) { _appSettings = settings; }
};

} // namespace walkinplace
