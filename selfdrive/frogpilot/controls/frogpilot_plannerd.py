import numpy as np

import cereal.messaging as messaging

from cereal import log

from openpilot.common.conversions import Conversions as CV
from openpilot.common.numpy_fast import interp
from openpilot.common.params import Params

from openpilot.selfdrive.car.interfaces import ACCEL_MIN, ACCEL_MAX
from openpilot.selfdrive.controls.lib.desire_helper import LANE_CHANGE_SPEED_MIN
from openpilot.selfdrive.controls.lib.drive_helpers import V_CRUISE_MAX
from openpilot.selfdrive.controls.lib.longitudinal_mpc_lib.long_mpc import COMFORT_BRAKE, STOP_DISTANCE, get_safe_obstacle_distance, get_stopped_equivalence_factor, get_T_FOLLOW
from openpilot.selfdrive.controls.lib.longitudinal_planner import A_CRUISE_MIN, get_max_accel

from openpilot.selfdrive.frogpilot.controls.lib.conditional_experimental_mode import ConditionalExperimentalMode
from openpilot.selfdrive.frogpilot.controls.lib.frogpilot_functions import CITY_SPEED_LIMIT, CRUISING_SPEED, calculate_lane_width, calculate_road_curvature
from openpilot.selfdrive.frogpilot.controls.lib.map_turn_speed_controller import MapTurnSpeedController
from openpilot.selfdrive.frogpilot.controls.lib.speed_limit_controller import SpeedLimitController

# Acceleration profiles - Credit goes to the DragonPilot team!
                 # MPH = [0., 18,  36,  63,  94]
A_CRUISE_MIN_BP_CUSTOM = [0., 8., 16., 28., 42.]
                 # MPH = [0., 6.71, 13.4, 17.9, 24.6, 33.6, 44.7, 55.9, 67.1, 123]
A_CRUISE_MAX_BP_CUSTOM = [0.,    3,   6.,   8.,  11.,  15.,  20.,  25.,  30., 55.]

A_CRUISE_MIN_VALS_ECO = [-0.001, -0.010, -0.28, -0.56, -0.56]
A_CRUISE_MAX_VALS_ECO = [3.5, 3.2, 2.3, 2.0, 1.15, .80, .58, .36, .30, .091]

A_CRUISE_MIN_VALS_SPORT = [-0.50, -0.52, -0.55, -0.57, -0.60]
A_CRUISE_MAX_VALS_SPORT = [3.5, 3.5, 3.3, 2.8, 1.5, 1.0, .75, .6, .38, .2]

TRAFFIC_MODE_BP = [0., CITY_SPEED_LIMIT]
TRAFFIC_MODE_T_FOLLOW = [.50, 1.]

TARGET_LAT_A = 1.9  # m/s^2

def get_min_accel_eco(v_ego):
  return interp(v_ego, A_CRUISE_MIN_BP_CUSTOM, A_CRUISE_MIN_VALS_ECO)

def get_max_accel_eco(v_ego):
  return interp(v_ego, A_CRUISE_MAX_BP_CUSTOM, A_CRUISE_MAX_VALS_ECO)

def get_min_accel_sport(v_ego):
  return interp(v_ego, A_CRUISE_MIN_BP_CUSTOM, A_CRUISE_MIN_VALS_SPORT)

def get_max_accel_sport(v_ego):
  return interp(v_ego, A_CRUISE_MAX_BP_CUSTOM, A_CRUISE_MAX_VALS_SPORT)

class FrogPilotPlannerd:
  def __init__(self, CP):
    self.CP = CP

    self.params = Params()
    self.params_memory = Params("/dev/shm/params")

    self.cem = ConditionalExperimentalMode()
    self.mtsc = MapTurnSpeedController()

    self.override_slc = False

    self.overridden_speed = 0
    self.mtsc_target = 0
    self.slc_target = 0
    self.t_follow = 0
    self.vtsc_target = 0

  def update(self, carState, controlsState, frogpilotCarControl, frogpilotNavigation, liveLocationKalman, modelData, radarState):
    v_cruise_kph = min(controlsState.vCruise, V_CRUISE_MAX)
    v_cruise = v_cruise_kph * CV.KPH_TO_MS
    v_ego = max(carState.vEgo, 0)
    v_lead = radarState.leadOne.vLead

    if self.acceleration_profile == 1:
      self.max_accel = get_max_accel_eco(v_ego)
    elif self.acceleration_profile in (2, 3):
      self.max_accel = get_max_accel_sport(v_ego)
    elif not controlsState.experimentalMode:
      self.max_accel = get_max_accel(v_ego)
    else:
      self.max_accel = ACCEL_MAX

    v_cruise_changed = (self.mtsc_target or self.vtsc_target) < v_cruise

    if self.deceleration_profile == 1 and not v_cruise_changed:
      self.min_accel = get_min_accel_eco(v_ego)
    elif self.deceleration_profile == 2 and not v_cruise_changed:
      self.min_accel = get_min_accel_sport(v_ego)
    elif not controlsState.experimentalMode:
      self.min_accel = A_CRUISE_MIN
    else:
      self.min_accel = ACCEL_MIN

    check_lane_width = self.adjacent_lanes or self.blind_spot_path or self.lane_detection
    if check_lane_width and v_ego >= LANE_CHANGE_SPEED_MIN:
      self.lane_width_left = float(calculate_lane_width(modelData.laneLines[0], modelData.laneLines[1], modelData.roadEdges[0]))
      self.lane_width_right = float(calculate_lane_width(modelData.laneLines[3], modelData.laneLines[2], modelData.roadEdges[1]))
    else:
      self.lane_width_left = 0
      self.lane_width_right = 0

    road_curvature = calculate_road_curvature(modelData, v_ego)

    if radarState.leadOne.status and self.CP.openpilotLongitudinalControl:
      base_t_follow = get_T_FOLLOW(self.custom_personalities, self.aggressive_follow, self.standard_follow, self.relaxed_follow, controlsState.personality)
      self.safe_obstacle_distance = int(np.mean(get_safe_obstacle_distance(v_ego, self.t_follow)))
      self.safe_obstacle_distance_stock = int(np.mean(get_safe_obstacle_distance(v_ego, base_t_follow)))
      self.stopped_equivalence_factor = int(np.mean(get_stopped_equivalence_factor(v_lead)))
    else:
      self.safe_obstacle_distance = 0
      self.safe_obstacle_distance_stock = 0
      self.stopped_equivalence_factor = 0

    self.t_follow = self.update_t_follow(controlsState, frogpilotCarControl, radarState, v_ego, v_lead)
    self.v_cruise = self.update_v_cruise(carState, controlsState, controlsState.enabled, liveLocationKalman, modelData, road_curvature, v_cruise, v_ego)

    if self.conditional_experimental_mode and self.CP.openpilotLongitudinalControl or self.green_light_alert:
      self.cem.update(carState, controlsState.enabled, frogpilotNavigation, modelData, radarState, road_curvature, self.t_follow, v_ego)

  def update_t_follow(self, controlsState, frogpilotCarControl, radarState, v_ego, v_lead):
    if self.traffic_mode and frogpilotCarControl.trafficModeActive:
      t_follow = interp(v_ego, TRAFFIC_MODE_BP, TRAFFIC_MODE_T_FOLLOW)
    else:
      t_follow = get_T_FOLLOW(self.custom_personalities, self.aggressive_follow, self.standard_follow, self.relaxed_follow, controlsState.personality)

    lead_distance = radarState.leadOne.dRel + (self.increased_stopping_distance if not frogpilotCarControl.trafficModeActive else 0)

    # Offset by FrogAi for FrogPilot for a more natural takeoff with a lead
    if self.aggressive_acceleration:
      distance_factor = np.maximum(1, lead_distance - (v_lead * t_follow))
      standstill_offset = max(STOP_DISTANCE - (v_ego**COMFORT_BRAKE), 0)
      acceleration_offset = np.clip((v_lead - v_ego) + standstill_offset - COMFORT_BRAKE, 1, distance_factor)
      t_follow = t_follow / acceleration_offset

    # Offset by FrogAi for FrogPilot for a more natural approach to a slower lead
    if self.smoother_braking:
      distance_factor = np.maximum(1, lead_distance - (v_lead * t_follow))
      braking_offset = np.clip((v_ego - v_lead) - COMFORT_BRAKE, 1, distance_factor)
      t_follow = t_follow / braking_offset

    return t_follow

  def update_v_cruise(self, carState, controlsState, enabled, liveLocationKalman, modelData, road_curvature, v_cruise, v_ego):
    gps_check = (liveLocationKalman.status == log.LiveLocationKalman.Status.valid) and liveLocationKalman.positionGeodetic.valid and liveLocationKalman.gpsOK

    v_cruise_cluster = max(controlsState.vCruiseCluster, controlsState.vCruise) * CV.KPH_TO_MS
    v_cruise_diff = v_cruise_cluster - v_cruise

    v_ego_cluster = max(carState.vEgoCluster, v_ego)
    v_ego_diff = v_ego_cluster - v_ego

    # Pfeiferj's Map Turn Speed Controller
    if self.map_turn_speed_controller and v_ego > CRUISING_SPEED and enabled and gps_check:
      mtsc_active = self.mtsc_target < v_cruise
      self.mtsc_target = np.clip(self.mtsc.target_speed(v_ego, carState.aEgo), CRUISING_SPEED, v_cruise)

      if self.mtsc_curvature_check and road_curvature < 1.0 and not mtsc_active:
        self.mtsc_target = v_cruise
    else:
      self.mtsc_target = v_cruise

    # Pfeiferj's Speed Limit Controller
    if self.speed_limit_controller:
      SpeedLimitController.update(v_ego)
      unconfirmed_slc_target = SpeedLimitController.desired_speed_limit

      # Check if the new speed limit has been confirmed by the user
      if self.speed_limit_confirmation:
        if self.params_memory.get_bool("SLCConfirmed") or self.slc_target == 0:
          self.slc_target = unconfirmed_slc_target
          self.params_memory.put_bool("SLCConfirmed", False)
      else:
        self.slc_target = unconfirmed_slc_target

      # Override SLC upon gas pedal press and reset upon brake/cancel button
      self.override_slc &= self.overridden_speed > self.slc_target
      self.override_slc |= carState.gasPressed and v_ego > self.slc_target
      self.override_slc &= enabled and not carState.standstill

      # Use the override speed if SLC is being overridden
      if self.override_slc:
        if self.speed_limit_controller_override == 1:
          # Set the speed limit to the manual set speed
          if carState.gasPressed:
            self.overridden_speed = v_ego + v_ego_diff
          self.overridden_speed = np.clip(self.overridden_speed, self.slc_target, v_cruise + v_cruise_diff)
        elif self.speed_limit_controller_override == 2:
          # Set the speed limit to the max set speed
          self.overridden_speed = v_cruise + v_cruise_diff
      else:
        self.overridden_speed = 0
    else:
      self.slc_target = v_cruise

    # Pfeiferj's Vision Turn Controller
    if self.vision_turn_controller and v_ego > CRUISING_SPEED and enabled:
      orientation_rate = np.array(np.abs(modelData.orientationRate.z)) * self.curve_sensitivity
      velocity = np.array(modelData.velocity.x)

      max_pred_lat_acc = np.amax(orientation_rate * velocity)
      max_curve = max_pred_lat_acc / (v_ego**2)
      adjusted_target_lat_a = TARGET_LAT_A * self.turn_aggressiveness

      self.vtsc_target = (adjusted_target_lat_a / max_curve)**0.5
      self.vtsc_target = np.clip(self.vtsc_target, CRUISING_SPEED, v_cruise)
    else:
      self.vtsc_target = v_cruise

    targets = [self.mtsc_target, max(self.overridden_speed, self.slc_target - v_ego_diff), self.vtsc_target]
    filtered_targets = [target if target > CRUISING_SPEED else v_cruise for target in targets]

    return min(filtered_targets)

  def publish(self, sm, pm):
    frogpilot_plan_send = messaging.new_message('frogpilotPlan')
    frogpilot_plan_send.valid = sm.all_checks(service_list=['carState', 'controlsState'])
    frogpilotPlan = frogpilot_plan_send.frogpilotPlan

    frogpilotPlan.adjustedCruise = float(min(self.mtsc_target, self.vtsc_target) * (CV.MS_TO_KPH if self.is_metric else CV.MS_TO_MPH))
    frogpilotPlan.conditionalExperimental = self.cem.experimental_mode

    frogpilotPlan.desiredFollowDistance = self.safe_obstacle_distance - self.stopped_equivalence_factor
    frogpilotPlan.safeObstacleDistance = self.safe_obstacle_distance
    frogpilotPlan.safeObstacleDistanceStock = self.safe_obstacle_distance_stock
    frogpilotPlan.stoppedEquivalenceFactor = self.stopped_equivalence_factor

    frogpilotPlan.laneWidthLeft = self.lane_width_left
    frogpilotPlan.laneWidthRight = self.lane_width_right
    frogpilotPlan.minAcceleration = self.min_accel
    frogpilotPlan.maxAcceleration = self.max_accel
    frogpilotPlan.tFollow = float(self.t_follow)
    frogpilotPlan.vCruise = float(self.v_cruise)

    frogpilotPlan.redLight = self.cem.red_light_detected

    frogpilotPlan.slcOverridden = bool(self.override_slc)
    frogpilotPlan.slcOverriddenSpeed = float(self.overridden_speed)
    frogpilotPlan.slcSpeedLimit = self.slc_target
    frogpilotPlan.slcSpeedLimitOffset = SpeedLimitController.offset
    frogpilotPlan.unconfirmedSlcSpeedLimit = SpeedLimitController.desired_speed_limit

    frogpilotPlan.vtscControllingCurve = bool(self.mtsc_target > self.vtsc_target)

    pm.send('frogpilotPlan', frogpilot_plan_send)

  def update_frogpilot_params(self):
    self.is_metric = self.params.get_bool("IsMetric")

    self.conditional_experimental_mode = self.CP.openpilotLongitudinalControl and self.params.get_bool("ConditionalExperimental")
    if self.conditional_experimental_mode:
      self.cem.update_frogpilot_params()

    custom_alerts = self.params.get_bool("CustomAlerts")
    self.green_light_alert = custom_alerts and self.params.get_bool("GreenLightAlert")

    self.custom_personalities = self.params.get_bool("CustomPersonalities")
    self.aggressive_follow = self.params.get_float("AggressiveFollow")
    self.standard_follow = self.params.get_float("StandardFollow")
    self.relaxed_follow = self.params.get_float("RelaxedFollow")

    custom_ui = self.params.get_bool("CustomUI")
    self.adjacent_lanes = custom_ui and self.params.get_bool("AdjacentPath")
    self.blind_spot_path = custom_ui and self.params.get_bool("BlindSpotPath")

    nudgeless_lane_change = self.params.get_bool("NudgelessLaneChange")
    self.lane_detection = nudgeless_lane_change and self.params.get_bool("LaneDetection")

    longitudinal_tune = self.CP.openpilotLongitudinalControl and self.params.get_bool("LongitudinalTune")
    self.acceleration_profile = self.params.get_int("AccelerationProfile") if longitudinal_tune else 0
    self.deceleration_profile = self.params.get_int("DecelerationProfile") if longitudinal_tune else 0
    self.aggressive_acceleration = longitudinal_tune and self.params.get_bool("AggressiveAcceleration")
    self.increased_stopping_distance = self.params.get_int("StoppingDistance") * (1 if self.is_metric else CV.FOOT_TO_METER) if longitudinal_tune else 0
    self.smoother_braking = longitudinal_tune and self.params.get_bool("SmoothBraking")
    self.traffic_mode = longitudinal_tune and self.params.get_bool("TrafficMode")

    self.map_turn_speed_controller = self.CP.openpilotLongitudinalControl and self.params.get_bool("MTSCEnabled")
    self.mtsc_curvature_check = self.map_turn_speed_controller and self.params.get_bool("MTSCCurvatureCheck")
    self.params_memory.put_float("MapTargetLatA", 2 * (self.params.get_int("MTSCAggressiveness") / 100))

    self.speed_limit_controller = self.CP.openpilotLongitudinalControl and self.params.get_bool("SpeedLimitController")
    self.speed_limit_confirmation = self.speed_limit_controller and self.params.get_bool("SLCConfirmation")
    self.speed_limit_controller_override = self.speed_limit_controller and self.params.get_int("SLCOverride")

    self.vision_turn_controller = self.CP.openpilotLongitudinalControl and self.params.get_bool("VisionTurnControl")
    self.curve_sensitivity = self.params.get_int("CurveSensitivity") / 100 if self.vision_turn_controller else 1
    self.turn_aggressiveness = self.params.get_int("TurnAggressiveness") / 100 if self.vision_turn_controller else 1
