#pragma once

#include <set>

#include "selfdrive/hpilot/ui/qt/widgets/hpilot_controls.h"
#include "selfdrive/ui/qt/offroad/settings.h"
#include "selfdrive/ui/ui.h"

class HpilotVisualsPanel : public HpilotListWidget {
  Q_OBJECT

public:
  explicit HpilotVisualsPanel(SettingsWindow *parent);

signals:
  void openParentToggle();

private:
  void hideToggles();
  void updateCarToggles();
  void updateMetric();
  void updateState(const UIState &s);
  void updateToggles();

  ButtonControl *mapStyleButton;

  std::set<QString> alertVolumeControlKeys = {"DisengageVolume", "EngageVolume", "PromptDistractedVolume", "PromptVolume", "RefuseVolume", "WarningImmediateVolume", "WarningSoftVolume"};
  std::set<QString> customAlertsKeys = {"GreenLightAlert", "LeadDepartingAlert", "LoudBlindspotAlert", "SpeedLimitChangedAlert"};
  std::set<QString> customOnroadUIKeys = {"Compass", "CustomPaths", "FPSCounter", "LeadInfo", "PedalsOnUI", "RoadNameUI", "WheelIcon", "ShowTorqueParams", "LeadLockon"};
  std::set<QString> customThemeKeys = {"CustomColors", "CustomIcons", "CustomSignals", "CustomSounds", "HolidayThemes", "RandomEvents"};
  std::set<QString> modelUIKeys = {"DynamicPathWidth", "HideLeadMarker", "LaneLinesWidth", "PathEdgeWidth", "PathWidth", "RoadEdgesWidth", "UnlimitedLength"};
  std::set<QString> qolKeys = {"BigMap", "CameraView", "DriverCamera", "FullMap", "HideSpeed", "MapStyle", "NumericalTemp", "WheelSpeed"};
  std::set<QString> screenKeys = {"HideUIElements", "ScreenBrightness", "ScreenBrightnessOnroad", "ScreenRecorder", "ScreenTimeout", "ScreenTimeoutOnroad", "StandbyMode"};

  std::map<std::string, ParamControl*> toggles;

  Params params;
  Params paramsMemory{"/dev/shm/params"};

  bool hasBSM;
  bool hasOpenpilotLongitudinal;
  bool isMetric = params.getBool("IsMetric");
  bool started;
};
