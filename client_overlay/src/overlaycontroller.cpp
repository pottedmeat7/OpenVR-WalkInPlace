#include "overlaycontroller.h"
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QQuickView>
#include <QApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsEllipseItem>
#include <QOpenGLExtraFunctions>
#include <QCursor>
#include <QProcess>
#include <QMessageBox>
#include <exception>
#include <iostream>
#include <cmath>
#include <openvr.h>
#include "logging.h"


// application namespace
namespace walkinplace {

	std::unique_ptr<OverlayController> OverlayController::singleton;

	QSettings* OverlayController::_appSettings = nullptr;

	OverlayController::~OverlayController() {
		Shutdown();
	}

	void OverlayController::Init(QQmlEngine* qmlEngine) {
		// Loading the OpenVR Runtime
		auto initError = vr::VRInitError_None;
		vr::VR_Init(&initError, vr::VRApplication_Overlay);
		if (initError != vr::VRInitError_None) {
			if (initError == vr::VRInitError_Init_HmdNotFound || initError == vr::VRInitError_Init_HmdNotFoundPresenceFailed) {
				QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "Could not find HMD!");
			}
			throw std::runtime_error(std::string("Failed to initialize OpenVR: ") + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(initError)));
		}

		static char rchBuffer[1024];
		uint32_t unRequiredSize;
		std::cout << vr::VR_GetRuntimePath(rchBuffer, sizeof(rchBuffer), &unRequiredSize);
		m_runtimePathUrl = QUrl::fromLocalFile(rchBuffer);
		LOG(INFO) << "VR Runtime Path: " << m_runtimePathUrl.toLocalFile();

		// Check whether OpenVR is too outdated
		if (!vr::VR_IsInterfaceVersionValid(vr::IVRSystem_Version)) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "OpenVR version is too outdated. Please update OpenVR.");
			throw std::runtime_error(std::string("OpenVR version is too outdated: Interface version ") + std::string(vr::IVRSystem_Version) + std::string(" not found."));
		}
		else if (!vr::VR_IsInterfaceVersionValid(vr::IVRSettings_Version)) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "OpenVR version is too outdated. Please update OpenVR.");
			throw std::runtime_error(std::string("OpenVR version is too outdated: Interface version ") + std::string(vr::IVRSettings_Version) + std::string(" not found."));
		}
		else if (!vr::VR_IsInterfaceVersionValid(vr::IVROverlay_Version)) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "OpenVR version is too outdated. Please update OpenVR.");
			throw std::runtime_error(std::string("OpenVR version is too outdated: Interface version ") + std::string(vr::IVROverlay_Version) + std::string(" not found."));
		}
		else if (!vr::VR_IsInterfaceVersionValid(vr::IVRApplications_Version)) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "OpenVR version is too outdated. Please update OpenVR.");
			throw std::runtime_error(std::string("OpenVR version is too outdated: Interface version ") + std::string(vr::IVRApplications_Version) + std::string(" not found."));
		}
		else if (!vr::VR_IsInterfaceVersionValid(vr::IVRChaperone_Version)) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "OpenVR version is too outdated. Please update OpenVR.");
			throw std::runtime_error(std::string("OpenVR version is too outdated: Interface version ") + std::string(vr::IVRChaperone_Version) + std::string(" not found."));
		}
		else if (!vr::VR_IsInterfaceVersionValid(vr::IVRChaperoneSetup_Version)) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "OpenVR version is too outdated. Please update OpenVR.");
			throw std::runtime_error(std::string("OpenVR version is too outdated: Interface version ") + std::string(vr::IVRChaperoneSetup_Version) + std::string(" not found."));
		}
		else if (!vr::VR_IsInterfaceVersionValid(vr::IVRCompositor_Version)) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "OpenVR version is too outdated. Please update OpenVR.");
			throw std::runtime_error(std::string("OpenVR version is too outdated: Interface version ") + std::string(vr::IVRCompositor_Version) + std::string(" not found."));
		}
		else if (!vr::VR_IsInterfaceVersionValid(vr::IVRNotifications_Version)) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "OpenVR version is too outdated. Please update OpenVR.");
			throw std::runtime_error(std::string("OpenVR version is too outdated: Interface version ") + std::string(vr::IVRNotifications_Version) + std::string(" not found."));
		}

		QSurfaceFormat format;
		// Qt's QOpenGLPaintDevice is not compatible with OpenGL versions >= 3.0
		// NVIDIA does not care, but unfortunately AMD does
		// Are subtle changes to the semantics of OpenGL functions actually covered by the compatibility profile,
		// and this is an AMD bug?
		format.setVersion(4, 1);
		format.setProfile(QSurfaceFormat::CompatibilityProfile);
		format.setDepthBufferSize(16);
		format.setStencilBufferSize(8);
		format.setSamples(16);

		m_pOpenGLContext.reset(new QOpenGLContext());
		m_pOpenGLContext->setFormat(format);
		if (!m_pOpenGLContext->create()) {
			throw std::runtime_error("Could not create OpenGL context");
		}

		// create an offscreen surface to attach the context and FBO to
		m_pOffscreenSurface.reset(new QOffscreenSurface());
		m_pOffscreenSurface->setFormat(m_pOpenGLContext->format());
		m_pOffscreenSurface->create();
		m_pOpenGLContext->makeCurrent(m_pOffscreenSurface.get());

		if (!vr::VROverlay()) {
			QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "Is OpenVR running?");
			throw std::runtime_error(std::string("No Overlay interface"));
		}

		// Init controllers
		walkInPlaceTabController.initStage1();

		// Set qml context
		qmlEngine->rootContext()->setContextProperty("applicationVersion", getVersionString());
		qmlEngine->rootContext()->setContextProperty("vrRuntimePath", getVRRuntimePathUrl());

		// Register qml singletons
		qmlRegisterSingletonType<OverlayController>("pottedmeat7.walkinplace", 1, 0, "OverlayController", [](QQmlEngine*, QJSEngine*) {
			QObject* obj = getInstance();
			QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
			return obj;
			});
		qmlRegisterSingletonType<WalkInPlaceTabController>("pottedmeat7.walkinplace", 1, 0, "WalkInPlaceTabController", [](QQmlEngine*, QJSEngine*) {
			QObject* obj = &getInstance()->walkInPlaceTabController;
			QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
			return obj;
			});
	}

	void OverlayController::Shutdown() {
		if (m_pPumpEventsTimer) {
			disconnect(m_pPumpEventsTimer.get(), SIGNAL(timeout()), this, SLOT(OnTimeoutPumpEvents()));
			m_pPumpEventsTimer->stop();
			m_pPumpEventsTimer.reset();
		}
		if (m_pRenderTimer) {
			disconnect(m_pRenderControl.get(), SIGNAL(renderRequested()), this, SLOT(OnRenderRequest()));
			disconnect(m_pRenderControl.get(), SIGNAL(sceneChanged()), this, SLOT(OnRenderRequest()));
			disconnect(m_pRenderTimer.get(), SIGNAL(timeout()), this, SLOT(renderOverlay()));
			m_pRenderTimer->stop();
			m_pRenderTimer.reset();
		}
		m_pWindow.reset();
		m_pRenderControl.reset();
		m_pFbo.reset();
		m_pOpenGLContext.reset();
		m_pOffscreenSurface.reset();
	}


	void OverlayController::SetWidget(QQuickItem* quickItem, const std::string& name, const std::string& key) {
		if (!desktopMode) {
			vr::VROverlayError overlayError = vr::VROverlay()->CreateDashboardOverlay(key.c_str(), name.c_str(), &m_ulOverlayHandle, &m_ulOverlayThumbnailHandle);
			if (overlayError != vr::VROverlayError_None) {
				if (overlayError == vr::VROverlayError_KeyInUse) {
					QMessageBox::critical(nullptr, "OpenVR WalkInPlace Overlay", "Another instance is already running.");
				}
				throw std::runtime_error(std::string("Failed to create Overlay: " + std::string(vr::VROverlay()->GetOverlayErrorNameFromEnum(overlayError))));
			}
			vr::VROverlay()->SetOverlayWidthInMeters(m_ulOverlayHandle, 2.5f);
			vr::VROverlay()->SetOverlayInputMethod(m_ulOverlayHandle, vr::VROverlayInputMethod_Mouse);
			//vr::VROverlay()->SetOverlayFlag(m_ulOverlayHandle, vr::VROverlayFlags_SendVRScrollEvents, true);
			std::string thumbIconPath = QApplication::applicationDirPath().toStdString() + slash + "res" + slash + "thumbicon.png";
			if (QFile::exists(QString::fromStdString(thumbIconPath))) {
				vr::VROverlay()->SetOverlayFromFile(m_ulOverlayThumbnailHandle, thumbIconPath.c_str());
			}
			else {
				LOG(ERROR) << "Could not find thumbnail icon \"" << thumbIconPath << "\"";
			}

			// Too many render calls in too short time overwhelm Qt and an assertion gets thrown.
			// Therefore we use an timer to delay render calls
			m_pRenderTimer.reset(new QTimer());
			m_pRenderTimer->setSingleShot(true);
			m_pRenderTimer->setInterval(5);
			connect(m_pRenderTimer.get(), SIGNAL(timeout()), this, SLOT(renderOverlay()));

			QOpenGLFramebufferObjectFormat fboFormat;
			fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
			fboFormat.setTextureTarget(GL_TEXTURE_2D);
			m_pFbo.reset(new QOpenGLFramebufferObject(quickItem->width(), quickItem->height(), fboFormat));

			m_pRenderControl.reset(new QQuickRenderControl());
			m_pWindow.reset(new QQuickWindow(m_pRenderControl.get()));
			m_pWindow->setRenderTarget(m_pFbo.get());
			quickItem->setParentItem(m_pWindow->contentItem());
			m_pWindow->setGeometry(0, 0, quickItem->width(), quickItem->height());
			m_pWindow->setFlags(Qt::FramelessWindowHint);
			m_pWindow->setColor(Qt::transparent);
			m_pRenderControl->initialize(m_pOpenGLContext.get());

			vr::HmdVector2_t vecWindowSize = {
				(float)quickItem->width(),
				(float)quickItem->height()
			};
			vr::VROverlay()->SetOverlayMouseScale(m_ulOverlayHandle, &vecWindowSize);

			connect(m_pRenderControl.get(), SIGNAL(renderRequested()), this, SLOT(OnRenderRequest()));
			connect(m_pRenderControl.get(), SIGNAL(sceneChanged()), this, SLOT(OnRenderRequest()));
		}

		m_pPumpEventsTimer.reset(new QTimer());
		connect(m_pPumpEventsTimer.get(), SIGNAL(timeout()), this, SLOT(OnTimeoutPumpEvents()));
		m_pPumpEventsTimer->setInterval(20);
		m_pPumpEventsTimer->start();

		walkInPlaceTabController.initStage2(this, m_pWindow.get());
	}


	void OverlayController::OnRenderRequest() {
		if (m_pRenderTimer && !m_pRenderTimer->isActive()) {
			m_pRenderTimer->start();
		}
	}

	void OverlayController::renderOverlay() {
		if (!desktopMode) {
			// skip rendering if the overlay isn't visible
			if (!vr::VROverlay() || !vr::VROverlay()->IsOverlayVisible(m_ulOverlayHandle) && !vr::VROverlay()->IsOverlayVisible(m_ulOverlayThumbnailHandle))
				return;
			m_pRenderControl->polishItems();
			m_pRenderControl->sync();
			m_pRenderControl->render();

			GLuint unTexture = m_pFbo->texture();
			if (unTexture != 0) {
#if defined _WIN64 || defined _LP64
				// To avoid any compiler warning because of cast to a larger pointer type (warning C4312 on VC)
				vr::Texture_t texture = { (void*)((uint64_t)unTexture), vr::TextureType_OpenGL, vr::ColorSpace_Auto };
#else
				vr::Texture_t texture = { (void*)unTexture, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
#endif
				vr::VROverlay()->SetOverlayTexture(m_ulOverlayHandle, &texture);
			}
			m_pOpenGLContext->functions()->glFlush(); // We need to flush otherwise the texture may be empty.*/
		}
	}


	bool OverlayController::getOverlayTexture(vr::Texture_t& texture) {
		GLuint unTexture = m_pFbo->texture();
		if (unTexture != 0) {
#if defined _WIN64 || defined _LP64
			// To avoid any compiler warning because of cast to a larger pointer type (warning C4312 on VC)
			texture = { (void*)((uint64_t)unTexture), vr::TextureType_OpenGL, vr::ColorSpace_Auto };
#else
			texture = { (void*)unTexture, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
#endif
			return true;
		}
		else {
			return false;
		}
	}


	void OverlayController::OnTimeoutPumpEvents() {
		if (!vr::VRSystem())
			return;

		vr::VREvent_t vrEvent;
		while (vr::VROverlay()->PollNextOverlayEvent(m_ulOverlayHandle, &vrEvent, sizeof(vrEvent))) {
			switch (vrEvent.eventType) {
			case vr::VREvent_MouseMove: {
#if defined (_WIN64) || !defined (_LP64)
				QPoint ptNewMouse(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
#else
				QPoint ptNewMouse(vrEvent.data.mouse.x, (m_pWindow->height() - vrEvent.data.mouse.y));
#endif
				if (ptNewMouse != m_ptLastMouse) {
					/*QPoint ptGlobal = ptNewMouse.toPoint();
					QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
					mouseEvent.setWidget(NULL);
					mouseEvent.setPos(ptNewMouse);
					mouseEvent.setScenePos(ptGlobal);
					mouseEvent.setScreenPos(ptGlobal);
					mouseEvent.setLastPos(m_ptLastMouse);
					mouseEvent.setLastScenePos(m_pWindow->mapToGlobal(m_ptLastMouse.toPoint()));
					mouseEvent.setLastScreenPos(m_pWindow->mapToGlobal(m_ptLastMouse.toPoint()));
					mouseEvent.setButtons(m_lastMouseButtons);
					mouseEvent.setButton(Qt::NoButton);
					mouseEvent.setModifiers(0);
					mouseEvent.setAccepted(false);*/

					m_ptLastMouse = ptNewMouse;
					//QApplication::sendEvent(m_pWindow.get(), &mouseEvent);
					QMouseEvent mE(QEvent::MouseMove, ptNewMouse, m_pWindow->mapToGlobal(ptNewMouse), Qt::NoButton, m_lastMouseButtons, 0);
					QCoreApplication::sendEvent(m_pWindow.get(), &mE);
					OnRenderRequest();
				}
				break;
			}
			case vr::VREvent_MouseButtonDown: {
				Qt::MouseButton button = vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;
				m_lastMouseButtons |= button;
#if defined (_WIN64) || !defined (_LP64)
				QPoint ptNewMouse(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
#else
				QPoint ptNewMouse(vrEvent.data.mouse.x, (m_pWindow->height() - vrEvent.data.mouse.y));
#endif
				QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
				mouseEvent.setWidget(NULL);
				mouseEvent.setPos(ptNewMouse);
				mouseEvent.setButtonDownPos(button, m_ptLastMouse);
				mouseEvent.setButtonDownScenePos(button, ptNewMouse);
				mouseEvent.setButtonDownScreenPos(button, ptNewMouse);
				mouseEvent.setScenePos(ptNewMouse);
				mouseEvent.setScreenPos(ptNewMouse);
				mouseEvent.setLastPos(m_ptLastMouse);
				mouseEvent.setLastScenePos(ptNewMouse);
				mouseEvent.setLastScreenPos(ptNewMouse);
				mouseEvent.setButtons(m_lastMouseButtons);
				mouseEvent.setButton(button);
				mouseEvent.setModifiers(0);
				mouseEvent.setAccepted(false);
				
				//QApplication::sendEvent(m_pWindow.get(), &mouseEvent);
				QMouseEvent mE(QEvent::MouseButtonPress, ptNewMouse, m_pWindow->mapToGlobal(ptNewMouse), button, m_lastMouseButtons, 0);
				QCoreApplication::sendEvent(m_pWindow.get(), &mE);
				break;
			}
			case vr::VREvent_MouseButtonUp: {
				Qt::MouseButton button = vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;
				m_lastMouseButtons &= ~button;
#if defined (_WIN64) || !defined (_LP64)
				QPoint ptNewMouse(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
#else
				QPoint ptNewMouse(vrEvent.data.mouse.x, (m_pWindow->height() - vrEvent.data.mouse.y));
#endif
				QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
				mouseEvent.setWidget(NULL);
				mouseEvent.setPos(m_ptLastMouse);
				mouseEvent.setScenePos(ptNewMouse);
				mouseEvent.setScreenPos(ptNewMouse);
				mouseEvent.setLastPos(m_ptLastMouse);
				mouseEvent.setLastScenePos(ptNewMouse);
				mouseEvent.setLastScreenPos(ptNewMouse);
				mouseEvent.setButtons(m_lastMouseButtons);
				mouseEvent.setButton(button);
				mouseEvent.setModifiers(0);
				mouseEvent.setAccepted(false);
				
				//QApplication::sendEvent(m_pWindow.get(), &mouseEvent);
				QMouseEvent mE(QEvent::MouseButtonRelease, ptNewMouse, m_pWindow->mapToGlobal(ptNewMouse), button, m_lastMouseButtons, 0);
				QCoreApplication::sendEvent(m_pWindow.get(), &mE);
				break;
			}
			case vr::VREvent_OverlayShown: {
				m_pWindow->update();
				break;
			}
			case vr::VREvent_Quit: {
				LOG(INFO) << "Received quit request.";
				vr::VRSystem()->AcknowledgeQuit_Exiting(); // Let us buy some time just in case
				Shutdown();
				QApplication::exit();
				return;
			}
			case vr::VREvent_DashboardActivated: {
				LOG(DEBUG) << "Dashboard activated";
				dashboardVisible = true;
				break;
			}
			case vr::VREvent_DashboardDeactivated: {
				LOG(DEBUG) << "Dashboard deactivated";
				dashboardVisible = false;
				break;
			}
			case vr::VREvent_KeyboardCharInput: {
				char keyboardBuffer[1024];
				vr::VROverlay()->GetKeyboardText(keyboardBuffer, 1024);
				emit keyBoardInputSignal(QString(keyboardBuffer), vrEvent.data.keyboard.uUserValue);
				break;
			}
			case vr::VREvent_KeyboardDone: {
				char keyboardBuffer[1024];
				vr::VROverlay()->GetKeyboardText(keyboardBuffer, 1024);
				emit keyBoardInputSignal(QString(keyboardBuffer), vrEvent.data.keyboard.uUserValue);
				break;
			}
			default:
				walkInPlaceTabController.handleEvent(vrEvent);
				break;
			}
		}

		walkInPlaceTabController.eventLoopTick();

		if (m_ulOverlayThumbnailHandle != vr::k_ulOverlayHandleInvalid) {
			while (vr::VROverlay()->PollNextOverlayEvent(m_ulOverlayThumbnailHandle, &vrEvent, sizeof(vrEvent))) {
				switch (vrEvent.eventType) {
				case vr::VREvent_OverlayShown: {
					m_pWindow->update();
					break;
				}
				}
			}
		}
	}

	QString OverlayController::getVersionString() {
		return QString(applicationVersionString);
	}


	QUrl OverlayController::getVRRuntimePathUrl() {
		return m_runtimePathUrl;
	}


	bool OverlayController::isDirectMode() {
		return directMode;
	}


	const vr::VROverlayHandle_t& OverlayController::overlayHandle() {
		return m_ulOverlayHandle;
	}


	const vr::VROverlayHandle_t& OverlayController::overlayThumbnailHandle() {
		return m_ulOverlayThumbnailHandle;
	}


	void OverlayController::showKeyboard(QString existingText, unsigned long userValue) {
		vr::VROverlay()->ShowKeyboardForOverlay(
			m_ulOverlayHandle,
			vr::k_EGamepadTextInputModeNormal,
			vr::k_EGamepadTextInputLineModeSingleLine,
			0,
			"Walk In Place Overlay",
			1024,
			existingText.toStdString().c_str(),
			userValue);
		vr::HmdVector2_t emptyvec;
		emptyvec.v[0] = 0;
		emptyvec.v[1] = 0;
		vr::HmdRect2_t empty;
		empty.vTopLeft = emptyvec;
		empty.vBottomRight = emptyvec;
		vr::VROverlay()->SetKeyboardPositionForOverlay(m_ulOverlayHandle, empty);
	}


} // namespace walkinplace
