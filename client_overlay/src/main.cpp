
#include "overlaycontroller.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QStyleFactory>
#include <QPalette>
#include <QSettings>
#include <QStandardPaths>
#include <QtCore/QObject>
#include <openvr.h>
#include <iostream>
#include <fstream>


#define LOG(text) std::cerr << text << std::endl;
#define ERROR(text) std::cerr << text << std::endl;

void myQtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {	
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type) {
		case QtDebugMsg:
			LOG(" (" << context.file << ":" << context.line << ")");
			break;
		case QtInfoMsg:
			LOG(" (" << context.file << ":" << context.line << ")");
			break;
		case QtWarningMsg:
			LOG(" (" << context.file << ":" << context.line << ")");
			break;
		case QtCriticalMsg:
			LOG(" (" << context.file << ":" << context.line << ")");
			break;
		case QtFatalMsg:
			LOG(" (" << context.file << ":" << context.line << ")");
			break;
	}
}

void installManifest(bool cleaninstall = false) {
	auto manifestQPath = QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("manifest.vrmanifest"));
	if (QFile::exists(manifestQPath)) {
		bool alreadyInstalled = false;
		if (vr::VRApplications()->IsApplicationInstalled(walkinplace::OverlayController::applicationKey)) {
			if (cleaninstall) {
				char buffer[1024];
				auto appError = vr::VRApplicationError_None;
				vr::VRApplications()->GetApplicationPropertyString(walkinplace::OverlayController::applicationKey, vr::VRApplicationProperty_WorkingDirectory_String, buffer, 1024, &appError);
				if (appError == vr::VRApplicationError_None) {
					auto oldManifestQPath = QDir::cleanPath(QDir(buffer).absoluteFilePath("manifest.vrmanifest"));
					if (oldManifestQPath.compare(manifestQPath, Qt::CaseInsensitive) != 0) {
						vr::VRApplications()->RemoveApplicationManifest(QDir::toNativeSeparators(oldManifestQPath).toStdString().c_str());
					} else {
						alreadyInstalled = true;
					}
				}
			} else {
				alreadyInstalled = true;
			}
		}
		auto apperror = vr::VRApplications()->AddApplicationManifest(QDir::toNativeSeparators(manifestQPath).toStdString().c_str());
		if (apperror != vr::VRApplicationError_None) {
			throw std::runtime_error(std::string("Could not add application manifest: ") + std::string(vr::VRApplications()->GetApplicationsErrorNameFromEnum(apperror)));
		} else if (!alreadyInstalled || cleaninstall) {
			auto apperror = vr::VRApplications()->SetApplicationAutoLaunch(walkinplace::OverlayController::applicationKey, true);
			if (apperror != vr::VRApplicationError_None) {
				throw std::runtime_error(std::string("Could not set auto start: ") + std::string(vr::VRApplications()->GetApplicationsErrorNameFromEnum(apperror)));
			}
		}
	} else {
		throw std::runtime_error(std::string("Could not find application manifest: ") + manifestQPath.toStdString());
	}
}

void removeManifest() {
	auto manifestQPath = QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("manifest.vrmanifest"));
	if (QFile::exists(manifestQPath)) {
		if (vr::VRApplications()->IsApplicationInstalled(walkinplace::OverlayController::applicationKey)) {
			vr::VRApplications()->RemoveApplicationManifest(QDir::toNativeSeparators(manifestQPath).toStdString().c_str());
		}
	} else {
		throw std::runtime_error(std::string("Could not find application manifest: ") + manifestQPath.toStdString());
	}
}


int main(int argc, char *argv[]) {

	bool desktopMode = false;
	bool directmode = false;
	bool noSound = true;
	bool noManifest = false;

	// Parse command line arguments
	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]).compare("-directmode") == 0) {
			directmode = true;
		} else if (std::string(argv[i]).compare("-desktop") == 0) {
			desktopMode = true;
		} else if (std::string(argv[i]).compare("-nonosound") == 0) {
			noSound = false;
		} else if (std::string(argv[i]).compare("-nomanifest") == 0) {
			noManifest = true;
		} else if (std::string(argv[i]).compare("-installmanifest") == 0) {
			std::this_thread::sleep_for(std::chrono::seconds(1)); // When we don't wait here we get might an ipc error during installation
			int exitcode = 0;
			QCoreApplication coreApp(argc, argv);
			auto initError = vr::VRInitError_None;
			vr::VR_Init(&initError, vr::VRApplication_Utility);
			if (initError == vr::VRInitError_None) {
				try {
					installManifest(true);
				} catch (std::exception& e) {
					exitcode = -1;
					std::cerr << e.what() << std::endl;
				}
			} else {
				exitcode = -2;
				ERROR("Failed to initialize OpenVR: " << std::string(vr::VR_GetVRInitErrorAsEnglishDescription(initError)));
			}
			vr::VR_Shutdown();
			exit(exitcode);
		} else if (std::string(argv[i]).compare("-removemanifest") == 0) {
			int exitcode = 0;
			QCoreApplication coreApp(argc, argv);
			auto initError = vr::VRInitError_None;
			vr::VR_Init(&initError, vr::VRApplication_Utility);
			if (initError == vr::VRInitError_None) {
				try {
					removeManifest();
				} catch (std::exception& e) {
					exitcode = -1;
					ERROR(e.what());
				}
			} else {
				exitcode = -2;
				ERROR(std::string("Failed to initialize OpenVR: " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(initError))));
			}
			vr::VR_Shutdown();
			exit(exitcode);
		} else if (std::string(argv[i]).compare("-openvrpath") == 0) {
			int exitcode = 0;
			auto initError = vr::VRInitError_None;
			vr::VR_Init(&initError, vr::VRApplication_Utility);
			if (initError == vr::VRInitError_None) {
				static char rchBuffer[1024];
				uint32_t unRequiredSize;
				vr::VR_GetRuntimePath(rchBuffer, sizeof(rchBuffer), &unRequiredSize);
				std::cout << rchBuffer;
			} else {
				exitcode = -2;
				ERROR(std::string("Failed to initialize OpenVR: " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(initError))));
			}
			vr::VR_Shutdown();
			exit(exitcode);
		} else if (std::string(argv[i]).compare("-postinstallationstep") == 0) {
			std::this_thread::sleep_for(std::chrono::seconds(1)); // When we don't wait here we get an ipc error during installation
			int exitcode = 0;
			auto initError = vr::VRInitError_None;
			vr::VR_Init(&initError, vr::VRApplication_Utility);
			if (initError == vr::VRInitError_None) {
				vr::VRSettings()->SetBool(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_ActivateMultipleDrivers_Bool, true);
			} else {
				exitcode = -2;
				ERROR(std::string("Failed to initialize OpenVR: " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(initError))));
			}
			vr::VR_Shutdown();
			exit(exitcode);
		}
	}

	try {
		QApplication a(argc, argv);
		a.setOrganizationName("pottedmeat7");
		a.setApplicationName("OpenVRWalkInPlace");
		a.setApplicationDisplayName(walkinplace::OverlayController::applicationName);
		a.setApplicationVersion(walkinplace::OverlayController::applicationVersionString);

		QFont fon("Helvetica", 17);
		a.setFont(fon);

		a.setStyle(QStyleFactory::create("Fusion"));

		QPalette darkPalette;
		darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
		darkPalette.setColor(QPalette::Mid, QColor(53, 53, 53));
		darkPalette.setColor(QPalette::WindowText, Qt::white);
		darkPalette.setColor(QPalette::Background, QColor(25, 25, 25));
		darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
		darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
		darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
		darkPalette.setColor(QPalette::ToolTipText, Qt::white);
		darkPalette.setColor(QPalette::Text, Qt::white);
		darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
		darkPalette.setColor(QPalette::ButtonText, Qt::white);
		darkPalette.setColor(QPalette::BrightText, Qt::red);
		darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

		darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
		darkPalette.setColor(QPalette::HighlightedText, Qt::black);

		a.setPalette(darkPalette);

		a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

		qInstallMessageHandler(myQtMessageHandler);

/*		// Configure logger
		QString logFilePath;
		el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
		auto logconfigfile = QFileInfo(logConfigFileName).absoluteFilePath();
		el::Configurations conf;
		if (QFile::exists(logconfigfile)) {
			conf.parseFromFile(logconfigfile.toStdString());
		} else {
			conf.parseFromText(logConfigDefault);
		}
		if (!conf.get(el::Level::Global, el::ConfigurationType::Filename)) {
			logFilePath = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absoluteFilePath("VRWalkInPlace.log");
			conf.set(el::Level::Global, el::ConfigurationType::Filename, QDir::toNativeSeparators(logFilePath).toStdString());
		}
		conf.setRemainingToDefault();
		el::Loggers::reconfigureAllLoggers(conf);
		LOG("Log Config: " + QDir::toNativeSeparators(logconfigfile).toStdString());
		if (!logFilePath.isEmpty()) {
			LOG("Log File: " + logFilePath);
		}
		*/
		LOG("Application started");
		
		if (desktopMode) {
			LOG("Desktop mode enabled.");
		}
		if (noSound) {
			LOG("Sound effects disabled.");
		}
		if (noManifest) {
			LOG("vrmanifest disabled.");
		}

		QSettings appSettings(QSettings::IniFormat, QSettings::UserScope, a.organizationName(), a.applicationName());
		walkinplace::OverlayController::setAppSettings(&appSettings);
		LOG("Settings File: " + appSettings.fileName().toStdString());

		QQmlEngine qmlEngine;

		walkinplace::OverlayController* controller = walkinplace::OverlayController::createInstance(desktopMode, noSound);
		controller->Init(&qmlEngine);

        QQmlComponent component(&qmlEngine, QUrl::fromLocalFile( QFileInfo("res/qml/mainwidget.qml").absoluteFilePath() ));
		auto errors = component.errors();
		for (auto& e : errors) {
			LOG("QML Error: " + e.toString().toStdString());
		}
		auto quickObj = component.create();
		controller->SetWidget(qobject_cast<QQuickItem*>(quickObj), walkinplace::OverlayController::applicationName, walkinplace::OverlayController::applicationKey);

		if (!desktopMode && !noManifest) {
			try {
				installManifest();
			} catch (std::exception& e) {
				LOG(e.what());
			}
		}

		if (desktopMode) {
			auto m_pWindow = new QQuickWindow();
			qobject_cast<QQuickItem*>(quickObj)->setParentItem(m_pWindow->contentItem());
			m_pWindow->setGeometry(0, 0, qobject_cast<QQuickItem*>(quickObj)->width(), qobject_cast<QQuickItem*>(quickObj)->height());
			//m_pWindow->setDefaultAlphaBuffer(true);
			//m_pWindow->setFlags(Qt::FramelessWindowHint);
			//m_pWindow->setColor(Qt::transparent);
			m_pWindow->show();

		}

		return a.exec();

	} catch (const std::exception& e) {
		LOG(e.what());
		return -1;
	}
	return 0;
}
