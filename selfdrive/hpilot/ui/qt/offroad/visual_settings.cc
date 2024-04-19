#include "selfdrive/hpilot/ui/qt/offroad/visual_settings.h"

HpilotVisualsPanel::HpilotVisualsPanel(SettingsWindow *parent) : HpilotListWidget(parent) {
  const std::vector<std::tuple<QString, QString, QString, QString>> visualToggles {
    {"AlertVolumeControl", tr("Alert Volume Controller"), tr("Control the volume level for each individual sound in openpilot."), "../hpilot/assets/toggle_icons/icon_mute.png"},
    {"DisengageVolume", tr("Disengage Volume"), tr("Related alerts:\n\nAdaptive Cruise Disabled\nParking Brake Engaged\nBrake Pedal Pressed\nSpeed too Low"), ""},
    {"EngageVolume", tr("Engage Volume"), tr("Related alerts:\n\nNNFF Torque Controller loaded\nopenpilot engaged"), ""},
    {"PromptVolume", tr("Prompt Volume"), tr("Related alerts:\n\nCar Detected in Blindspot\nSpeed too Low\nSteer Unavailable Below 'X'\nTake Control, Turn Exceeds Steering Limit"), ""},
    {"PromptDistractedVolume", tr("Prompt Distracted Volume"), tr("Related alerts:\n\nPay Attention, Driver Distracted\nTouch Steering Wheel, Driver Unresponsive"), ""},
    {"RefuseVolume", tr("Refuse Volume"), tr("Related alerts:\n\nopenpilot Unavailable"), ""},
    {"WarningSoftVolume", tr("Warning Soft Volume"), tr("Related alerts:\n\nBRAKE!, Risk of Collision\nTAKE CONTROL IMMEDIATELY"), ""},
    {"WarningImmediateVolume", tr("Warning Immediate Volume"), tr("Related alerts:\n\nDISENGAGE IMMEDIATELY, Driver Distracted\nDISENGAGE IMMEDIATELY, Driver Unresponsive"), ""},

    {"CustomAlerts", tr("Custom Alerts"), tr("Enable custom alerts for openpilot events."), "../hpilot/assets/toggle_icons/icon_green_light.png"},
    {"GreenLightAlert", tr("Green Light Alert"), tr("Get an alert when a traffic light changes from red to green."), ""},
    {"LeadDepartingAlert", tr("Lead Departing Alert"), tr("Get an alert when the lead vehicle starts departing when at a standstill."), ""},
    {"LoudBlindspotAlert", tr("Loud Blindspot Alert"), tr("Enable a louder alert for when a vehicle is detected in the blindspot when attempting to change lanes."), ""},
    {"SpeedLimitChangedAlert", tr("Speed Limit Changed Alert"), tr("Trigger an alert whenever the speed limit changes."), ""},

    {"CustomUI", tr("Custom Onroad UI"), tr("Customize the Onroad UI."), "../assets/offroad/icon_road.png"},
    {"Compass", tr("Compass"), tr("Add a compass to the onroad UI."), ""},
    {"FPSCounter", tr("FPS Counter"), tr("Display the 'Frames Per Second' (FPS) of your onroad UI for monitoring system performance."), ""},
    {"LeadInfo", tr("Lead Info and Logics"), tr("Get detailed information about the vehicle ahead, including speed and distance, and the logic behind your following distance."), ""},
    {"CustomPaths", tr("Paths"), tr("Show your projected acceleration on the driving path, detected adjacent lanes, or when a vehicle is detected in your blindspot."), ""},
    {"PedalsOnUI", tr("Pedals Being Pressed"), tr("Display the brake and gas pedals on the onroad UI below the steering wheel icon."), ""},
    {"RoadNameUI", tr("Road Name"), tr("Display the current road's name at the bottom of the screen. Sourced from OpenStreetMap."), ""},
    {"WheelIcon", tr("Steering Wheel Icon"), tr("Replace the default steering wheel icon with a custom icon."), ""},
    {"ShowTorqueParams", tr("Torque Parameters"), tr("Display the lateral acceleration factor and friction coefficient values from the torque controller in the onroad UI."), ""},

    {"CustomTheme", tr("Custom Themes"), tr("Enable the ability to use custom themes."), "../hpilot/assets/wheel_images/frog.png"},
    {"CustomColors", tr("Color Theme"), tr("Switch out the standard openpilot color scheme with themed colors.\n\nWant to submit your own color scheme? Post it in the 'feature-request' channel in the Hpilot Discord!"), ""},
    {"CustomIcons", tr("Icon Pack"), tr("Switch out the standard openpilot icons with a set of themed icons.\n\nWant to submit your own icon pack? Post it in the 'feature-request' channel in the Hpilot Discord!"), ""},
    {"CustomSignals", tr("Turn Signals"), tr("Add themed animation for your turn signals.\n\nWant to submit your own turn signal animation? Post it in the 'feature-request' channel in the Hpilot Discord!"), ""},
    {"CustomSounds", tr("Sound Pack"), tr("Switch out the standard openpilot sounds with a set of themed sounds.\n\nWant to submit your own sound pack? Post it in the 'feature-request' channel in the Hpilot Discord!"), ""},
    {"HolidayThemes", tr("Holiday Themes"), tr("The openpilot theme changes according to the current/upcoming holiday. Minor holidays last a day, while major holidays (Easter, Christmas, Halloween, etc.) last a week."), ""},
    {"RandomEvents", tr("Random Events"), tr("Enjoy a bit of unpredictability with random events that can occur during certain driving conditions. This is purely cosmetic and has no impact on driving controls!"), ""},

    {"ModelUI", tr("Model UI"), tr("Customize the model visualizations on the screen."), "../assets/offroad/icon_calibration.png"},
    {"DynamicPathWidth", tr("Dynamic Path Width"), tr("Have the path width dynamically adjust based on the current engagement state of openpilot."), ""},
    {"HideLeadMarker", tr("Hide Lead Marker"), tr("Hide the lead marker from the onroad UI."), ""},
    {"LaneLinesWidth", tr("Lane Lines"), tr("Adjust the visual thickness of lane lines on your display.\n\nDefault matches the MUTCD average of 4 inches."), ""},
    {"PathEdgeWidth", tr("Path Edges"), tr("Adjust the width of the path edges shown on your UI to represent different driving modes and statuses.\n\nDefault is 20% of the total path.\n\nBlue = Navigation\nLight Blue = 'Always On Lateral'\nGreen = Default\nOrange = 'Experimental Mode'\nRed = 'Traffic Mode'\nYellow = 'Conditional Experimental Mode' Overriden"), ""},
    {"PathWidth", tr("Path Width"), tr("Customize the width of the driving path shown on your UI.\n\nDefault matches the width of a 2019 Lexus ES 350."), ""},
    {"RoadEdgesWidth", tr("Road Edges"), tr("Adjust the visual thickness of road edges on your display.\n\nDefault is 1/2 of the MUTCD average lane line width of 4 inches."), ""},
    {"UnlimitedLength", tr("'Unlimited' Road UI Length"), tr("Extend the display of the path, lane lines, and road edges out as far as the model can see."), ""},

    {"QOLVisuals", tr("Quality of Life"), tr("Miscellaneous quality of life changes to improve your overall openpilot experience."), "../hpilot/assets/toggle_icons/quality_of_life.png"},
    {"BigMap", tr("Big Map"), tr("Increase the size of the map in the onroad UI."), ""},
    {"CameraView", tr("Camera View"), tr("Choose your preferred camera view for the onroad UI. This is purely a visual change and doesn't impact how openpilot drives."), ""},
    {"DriverCamera", tr("Driver Camera On Reverse"), tr("Show the driver camera feed when in reverse."), ""},
    {"HideSpeed", tr("Hide Speed"), tr("Hide the speed indicator in the onroad UI. Additional toggle allows it to be hidden/shown via tapping the speed itself."), ""},
    {"MapStyle", tr("Map Style"), tr("Select a map style to use with navigation."), ""},
    {"NumericalTemp", tr("Numerical Temperature Gauge"), tr("Replace the 'GOOD', 'OK', and 'HIGH' temperature statuses with a numerical temperature gauge based on the highest temperature between the memory, CPU, and GPU."), ""},
    {"WheelSpeed", tr("Use Wheel Speed"), tr("Use the wheel speed instead of the cluster speed in the onroad UI."), ""},

    {"ScreenManagement", tr("Screen Management"), tr("Manage your screen's brightness, timeout settings, and hide onroad UI elements."), "../hpilot/assets/toggle_icons/icon_light.png"},
    {"HideUIElements", tr("Hide UI Elements"), tr("Hide the selected UI elements from the onroad screen."), ""},
    {"ScreenBrightness", tr("Screen Brightness"), tr("Customize your screen brightness when offroad."), ""},
    {"ScreenBrightnessOnroad", tr("Screen Brightness (Onroad)"), tr("Customize your screen brightness when onroad."), ""},
    {"ScreenRecorder", tr("Screen Recorder"), tr("Enable the ability to record the screen while onroad."), ""},
    {"ScreenTimeout", tr("Screen Timeout"), tr("Customize how long it takes for your screen to turn off."), ""},
    {"ScreenTimeoutOnroad", tr("Screen Timeout (Onroad)"), tr("Customize how long it takes for your screen to turn off when onroad."), ""},
    {"StandbyMode", tr("Standby Mode"), tr("Turn the screen off after your screen times out when onroad, but wake it back up when engagement state changes or important alerts are triggered."), ""},
  };

  for (const auto &[param, title, desc, icon] : visualToggles) {
    ParamControl *toggle;

    if (param == "AlertVolumeControl") {
      HpilotParamManageControl *alertVolumeControlToggle = new HpilotParamManageControl(param, title, desc, icon, this);
      QObject::connect(alertVolumeControlToggle, &HpilotParamManageControl::manageButtonClicked, this, [this]() {
        openParentToggle();
        for (auto &[key, toggle] : toggles) {
          toggle->setVisible(alertVolumeControlKeys.find(key.c_str()) != alertVolumeControlKeys.end());
        }
      });
      toggle = alertVolumeControlToggle;
    } else if (alertVolumeControlKeys.find(param) != alertVolumeControlKeys.end()) {
      if (param == "WarningImmediateVolume") {
        toggle = new HpilotParamValueControl(param, title, desc, icon, 25, 100, std::map<int, QString>(), this, false, "%");
      } else {
        toggle = new HpilotParamValueControl(param, title, desc, icon, 0, 100, std::map<int, QString>(), this, false, "%");
      }

    } else if (param == "CustomAlerts") {
      HpilotParamManageControl *customAlertsToggle = new HpilotParamManageControl(param, title, desc, icon, this);
      QObject::connect(customAlertsToggle, &HpilotParamManageControl::manageButtonClicked, this, [this]() {
        openParentToggle();
        for (auto &[key, toggle] : toggles) {
          std::set<QString> modifiedCustomAlertsKeys = customAlertsKeys;

          if (!hasBSM) {
            modifiedCustomAlertsKeys.erase("LoudBlindspotAlert");
          }

          if (!hasOpenpilotLongitudinal) {
            modifiedCustomAlertsKeys.erase("SpeedLimitChangedAlert");
          }

          toggle->setVisible(modifiedCustomAlertsKeys.find(key.c_str()) != modifiedCustomAlertsKeys.end());
        }
      });
      toggle = customAlertsToggle;

    } else if (param == "CustomTheme") {
      HpilotParamManageControl *customThemeToggle = new HpilotParamManageControl(param, title, desc, icon, this);
      QObject::connect(customThemeToggle, &HpilotParamManageControl::manageButtonClicked, this, [this]() {
        openParentToggle();
        for (auto &[key, toggle] : toggles) {
          toggle->setVisible(customThemeKeys.find(key.c_str()) != customThemeKeys.end());
        }
      });
      toggle = customThemeToggle;
    } else if (param == "CustomColors" || param == "CustomIcons" || param == "CustomSignals" || param == "CustomSounds") {
      std::vector<QString> themeOptions{tr("Stock"), tr("Frog"), tr("Tesla"), tr("Stalin")};
      HpilotButtonParamControl *themeSelection = new HpilotButtonParamControl(param, title, desc, icon, themeOptions);
      toggle = themeSelection;

      if (param == "CustomSounds") {
        QObject::connect(static_cast<HpilotButtonParamControl*>(toggle), &HpilotButtonParamControl::buttonClicked, [this](int id) {
          if (id == 1) {
            if (HpilotConfirmationDialog::yesorno(tr("Do you want to enable the bonus 'Goat' sound effect?"), this)) {
              params.putBoolNonBlocking("GoatScream", true);
            } else {
              params.putBoolNonBlocking("GoatScream", false);
            }
          }
        });
      }

    } else if (param == "CustomUI") {
      HpilotParamManageControl *customUIToggle = new HpilotParamManageControl(param, title, desc, icon, this);
      QObject::connect(customUIToggle, &HpilotParamManageControl::manageButtonClicked, this, [this]() {
        openParentToggle();
        for (auto &[key, toggle] : toggles) {
          std::set<QString> modifiedCustomOnroadUIKeys = customOnroadUIKeys;

          if (!hasOpenpilotLongitudinal) {
            modifiedCustomOnroadUIKeys.erase("LeadInfo");
          }

          toggle->setVisible(modifiedCustomOnroadUIKeys.find(key.c_str()) != modifiedCustomOnroadUIKeys.end());
        }
      });
      toggle = customUIToggle;
    } else if (param == "CustomPaths") {
      std::vector<QString> pathToggles{"AccelerationPath", "AdjacentPath", "BlindSpotPath", "AdjacentPathMetrics"};
      std::vector<QString> pathToggleNames{tr("Acceleration"), tr("Adjacent"), tr("Blind Spot"), tr("Metrics")};
      toggle = new HpilotParamToggleControl(param, title, desc, icon, pathToggles, pathToggleNames);
    } else if (param == "LeadInfo") {
      std::vector<QString> leadInfoToggles{"UseSI"};
      std::vector<QString> leadInfoToggleNames{tr("Use SI Values")};
      toggle = new HpilotParamToggleControl(param, title, desc, icon, leadInfoToggles, leadInfoToggleNames);
    } else if (param == "WheelIcon") {
      std::vector<QString> wheelToggles{"RotatingWheel"};
      std::vector<QString> wheelToggleNames{"Live Rotation"};
      std::map<int, QString> steeringWheelLabels = {{-1, tr("None")}, {0, tr("Stock")}, {1, tr("Lexus")}, {2, tr("Toyota")}, {3, tr("Frog")}, {4, tr("Rocket")}, {5, tr("Hyundai")}, {6, tr("Stalin")}};
      toggle = new HpilotParamValueToggleControl(param, title, desc, icon, -1, 6, steeringWheelLabels, this, true, "", 1, 1, wheelToggles, wheelToggleNames);

    } else if (param == "ModelUI") {
      HpilotParamManageControl *modelUIToggle = new HpilotParamManageControl(param, title, desc, icon, this);
      QObject::connect(modelUIToggle, &HpilotParamManageControl::manageButtonClicked, this, [this]() {
        openParentToggle();
        for (auto &[key, toggle] : toggles) {
          std::set<QString> modifiedModelUIKeysKeys = modelUIKeys;

          if (!hasOpenpilotLongitudinal) {
            modifiedModelUIKeysKeys.erase("HideLeadMarker");
          }

          toggle->setVisible(modifiedModelUIKeysKeys.find(key.c_str()) != modifiedModelUIKeysKeys.end());
        }
      });
      toggle = modelUIToggle;
    } else if (param == "LaneLinesWidth" || param == "RoadEdgesWidth") {
      toggle = new HpilotParamValueControl(param, title, desc, icon, 0, 24, std::map<int, QString>(), this, false, tr(" inches"));
    } else if (param == "PathEdgeWidth") {
      toggle = new HpilotParamValueControl(param, title, desc, icon, 0, 100, std::map<int, QString>(), this, false, tr("%"));
    } else if (param == "PathWidth") {
      toggle = new HpilotParamValueControl(param, title, desc, icon, 0, 100, std::map<int, QString>(), this, false, tr(" feet"), 10);

    } else if (param == "QOLVisuals") {
      HpilotParamManageControl *qolToggle = new HpilotParamManageControl(param, title, desc, icon, this);
      QObject::connect(qolToggle, &HpilotParamManageControl::manageButtonClicked, this, [this]() {
        openParentToggle();
        for (auto &[key, toggle] : toggles) {
          toggle->setVisible(qolKeys.find(key.c_str()) != qolKeys.end());
        }
        mapStyleButton->setVisible(true);
      });
      toggle = qolToggle;
    } else if (param == "CameraView") {
      std::vector<QString> cameraOptions{tr("Auto"), tr("Driver"), tr("Standard"), tr("Wide")};
      HpilotButtonParamControl *preferredCamera = new HpilotButtonParamControl(param, title, desc, icon, cameraOptions);
      toggle = preferredCamera;
    } else if (param == "BigMap") {
      std::vector<QString> mapToggles{"FullMap"};
      std::vector<QString> mapToggleNames{tr("Full Map")};
      toggle = new HpilotParamToggleControl(param, title, desc, icon, mapToggles, mapToggleNames);
    } else if (param == "HideSpeed") {
      std::vector<QString> hideSpeedToggles{"HideSpeedUI"};
      std::vector<QString> hideSpeedToggleNames{tr("Control Via UI")};
      toggle = new HpilotParamToggleControl(param, title, desc, icon, hideSpeedToggles, hideSpeedToggleNames);
    } else if (param == "MapStyle") {
      QMap<int, QString> styleMap = {
        {0, tr("Stock openpilot")},
        {1, tr("Mapbox Streets")},
        {2, tr("Mapbox Outdoors")},
        {3, tr("Mapbox Light")},
        {4, tr("Mapbox Dark")},
        {5, tr("Mapbox Satellite")},
        {6, tr("Mapbox Satellite Streets")},
        {7, tr("Mapbox Navigation Day")},
        {8, tr("Mapbox Navigation Night")},
        {9, tr("Mapbox Traffic Night")},
        {10, tr("mike854's (Satellite hybrid)")},
      };

      QStringList styles = styleMap.values();

      mapStyleButton = new ButtonControl(title, tr("SELECT"), desc);
      QObject::connect(mapStyleButton, &ButtonControl::clicked, this, [this, styleMap]() {
        QStringList styles = styleMap.values();
        QString selection = MultiOptionDialog::getSelection(tr("Select a map style"), styles, "", this);
        if (!selection.isEmpty()) {
          int selectedStyle = styleMap.key(selection);
          params.putInt("MapStyle", selectedStyle);
          mapStyleButton->setValue(selection);
          updateToggles();
        }
      });

      int currentStyle = params.getInt("MapStyle");
      mapStyleButton->setValue(styleMap[currentStyle]);

      addItem(mapStyleButton);
    } else if (param == "NumericalTemp") {
      std::vector<QString> temperatureToggles{"Fahrenheit"};
      std::vector<QString> temperatureToggleNames{tr("Fahrenheit")};
      toggle = new HpilotParamToggleControl(param, title, desc, icon, temperatureToggles, temperatureToggleNames);

    } else if (param == "ScreenManagement") {
      HpilotParamManageControl *screenToggle = new HpilotParamManageControl(param, title, desc, icon, this);
      QObject::connect(screenToggle, &HpilotParamManageControl::manageButtonClicked, this, [this]() {
        openParentToggle();
        for (auto &[key, toggle] : toggles) {
          toggle->setVisible(screenKeys.find(key.c_str()) != screenKeys.end());
        }
      });
      toggle = screenToggle;
    } else if (param == "HideUIElements") {
      std::vector<QString> uiElementsToggles{"HideAlerts", "HideMapIcon", "HideMaxSpeed"};
      std::vector<QString> uiElementsToggleNames{tr("Alerts"), tr("Map Icon"), tr("Max Speed")};
      toggle = new HpilotParamToggleControl(param, title, desc, icon, uiElementsToggles, uiElementsToggleNames);
    } else if (param == "ScreenBrightness" || param == "ScreenBrightnessOnroad") {
      std::map<int, QString> brightnessLabels;
      for (int i = 0; i <= 101; ++i) {
        brightnessLabels[i] = i == 0 ? tr("Screen Off") : i == 101 ? tr("Auto") : QString::number(i) + "%";
      }
      toggle = new HpilotParamValueControl(param, title, desc, icon, 0, 101, brightnessLabels, this, false);
    } else if (param == "ScreenTimeout" || param == "ScreenTimeoutOnroad") {
      toggle = new HpilotParamValueControl(param, title, desc, icon, 5, 60, std::map<int, QString>(), this, false, tr(" seconds"));

    } else {
      toggle = new ParamControl(param, title, desc, icon, this);
    }

    addItem(toggle);
    toggles[param.toStdString()] = toggle;

    QObject::connect(toggle, &ToggleControl::toggleFlipped, [this]() {
      updateToggles();
    });

    QObject::connect(static_cast<HpilotButtonParamControl*>(toggle), &HpilotButtonParamControl::buttonClicked, [this]() {
      updateToggles();
    });

    QObject::connect(static_cast<HpilotParamValueControl*>(toggle), &HpilotParamValueControl::valueChanged, [this]() {
      updateToggles();
    });

    QObject::connect(toggle, &AbstractControl::showDescriptionEvent, [this]() {
      update();
    });

    QObject::connect(static_cast<HpilotParamManageControl*>(toggle), &HpilotParamManageControl::manageButtonClicked, [this]() {
      update();
    });
  }

  QObject::connect(parent, &SettingsWindow::closeParentToggle, this, &HpilotVisualsPanel::hideToggles);
  QObject::connect(parent, &SettingsWindow::updateMetric, this, &HpilotVisualsPanel::updateMetric);
  QObject::connect(uiState(), &UIState::offroadTransition, this, &HpilotVisualsPanel::updateCarToggles);
  QObject::connect(uiState(), &UIState::uiUpdate, this, &HpilotVisualsPanel::updateState);

  updateMetric();
}

void HpilotVisualsPanel::updateState(const UIState &s) {
  if (!isVisible()) return;

  started = s.scene.started;
}

void HpilotVisualsPanel::hideEvent(QHideEvent *event) {
  hideToggles();
}

void HpilotVisualsPanel::updateToggles() {
  std::thread([this]() {
    paramsMemory.putBool("HpilotTogglesUpdated", true);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    paramsMemory.putBool("HpilotTogglesUpdated", false);
  }).detach();
}

void HpilotVisualsPanel::updateCarToggles() {
  auto carParams = params.get("CarParamsPersistent");
  if (!carParams.empty()) {
    AlignedBuffer aligned_buf;
    capnp::FlatArrayMessageReader cmsg(aligned_buf.align(carParams.data(), carParams.size()));
    cereal::CarParams::Reader CP = cmsg.getRoot<cereal::CarParams>();

    hasBSM = CP.getEnableBsm();
    hasOpenpilotLongitudinal = CP.getOpenpilotLongitudinalControl();
  } else {
    hasBSM = true;
    hasOpenpilotLongitudinal = true;
  }

  hideToggles();
}

void HpilotVisualsPanel::updateMetric() {
  bool previousIsMetric = isMetric;
  isMetric = params.getBool("IsMetric");

  if (isMetric != previousIsMetric) {
    double distanceConversion = isMetric ? INCH_TO_CM : CM_TO_INCH;
    double speedConversion = isMetric ? FOOT_TO_METER : METER_TO_FOOT;
    params.putIntNonBlocking("LaneLinesWidth", std::nearbyint(params.getInt("LaneLinesWidth") * distanceConversion));
    params.putIntNonBlocking("RoadEdgesWidth", std::nearbyint(params.getInt("RoadEdgesWidth") * distanceConversion));
    params.putIntNonBlocking("PathWidth", std::nearbyint(params.getInt("PathWidth") * speedConversion));
  }

  HpilotParamValueControl *laneLinesWidthToggle = static_cast<HpilotParamValueControl*>(toggles["LaneLinesWidth"]);
  HpilotParamValueControl *roadEdgesWidthToggle = static_cast<HpilotParamValueControl*>(toggles["RoadEdgesWidth"]);
  HpilotParamValueControl *pathWidthToggle = static_cast<HpilotParamValueControl*>(toggles["PathWidth"]);

  if (isMetric) {
    laneLinesWidthToggle->setDescription(tr("Customize the lane line width.\n\nDefault matches the Vienna average of 10 centimeters."));
    roadEdgesWidthToggle->setDescription(tr("Customize the road edges width.\n\nDefault is 1/2 of the Vienna average lane line width of 10 centimeters."));

    laneLinesWidthToggle->updateControl(0, 60, tr(" centimeters"));
    roadEdgesWidthToggle->updateControl(0, 60, tr(" centimeters"));
    pathWidthToggle->updateControl(0, 30, tr(" meters"), 10);
  } else {
    laneLinesWidthToggle->setDescription(tr("Customize the lane line width.\n\nDefault matches the MUTCD average of 4 inches."));
    roadEdgesWidthToggle->setDescription(tr("Customize the road edges width.\n\nDefault is 1/2 of the MUTCD average lane line width of 4 inches."));

    laneLinesWidthToggle->updateControl(0, 24, tr(" inches"));
    roadEdgesWidthToggle->updateControl(0, 24, tr(" inches"));
    pathWidthToggle->updateControl(0, 100, tr(" feet"), 10);
  }

  laneLinesWidthToggle->refresh();
  roadEdgesWidthToggle->refresh();
}

void HpilotVisualsPanel::hideToggles() {
  mapStyleButton->setVisible(false);

  for (auto &[key, toggle] : toggles) {
    bool subToggles = alertVolumeControlKeys.find(key.c_str()) != alertVolumeControlKeys.end() ||
                      customAlertsKeys.find(key.c_str()) != customAlertsKeys.end() ||
                      customOnroadUIKeys.find(key.c_str()) != customOnroadUIKeys.end() ||
                      customThemeKeys.find(key.c_str()) != customThemeKeys.end() ||
                      modelUIKeys.find(key.c_str()) != modelUIKeys.end() ||
                      qolKeys.find(key.c_str()) != qolKeys.end() ||
                      screenKeys.find(key.c_str()) != screenKeys.end();
    toggle->setVisible(!subToggles);
  }

  update();
}