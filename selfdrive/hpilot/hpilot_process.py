import datetime
import http.client
import os
import socket
import time
import urllib.error
import urllib.request

import cereal.messaging as messaging

from cereal import car, log
from openpilot.common.params import Params
from openpilot.common.realtime import DT_MDL, Priority, config_realtime_process
from openpilot.common.time import system_time_valid
from openpilot.system.hardware import HARDWARE

from openpilot.selfdrive.hpilot.controls.hpilot_plannerd import HpilotPlannerd
from openpilot.selfdrive.hpilot.controls.lib.hpilot_functions import HpilotFunctions
from openpilot.selfdrive.hpilot.controls.lib.model_manager import DEFAULT_MODEL, DEFAULT_MODEL_NAME, download_model, populate_models
from openpilot.selfdrive.hpilot.controls.lib.theme_manager import ThemeManager

WIFI = log.DeviceState.NetworkType.wifi

def automatic_update_check(params):
  update_available = params.get_bool("UpdaterFetchAvailable")
  update_ready = params.get_bool("UpdateAvailable")
  update_state = params.get("UpdaterState", encoding='utf8')

  if update_ready:
    HARDWARE.reboot()
  elif update_available:
    os.system("pkill -SIGHUP -f selfdrive.updated.updated")
  elif update_state == "idle":
    os.system("pkill -SIGUSR1 -f selfdrive.updated.updated")

def github_pinged(url="https://github.com", timeout=5):
  try:
    urllib.request.urlopen(url, timeout=timeout)
    return True
  except (urllib.error.URLError, socket.timeout, http.client.RemoteDisconnected):
    return False

def time_checks(automatic_updates, deviceState, params):
  if github_pinged():
    populate_models()

    screen_off = deviceState.screenBrightnessPercent == 0
    wifi_connection = deviceState.networkType == WIFI

    if automatic_updates and screen_off and wifi_connection:
      automatic_update_check(params)

def hpilot_thread():
  config_realtime_process(5, Priority.CTRL_LOW)

  params = Params()
  params_memory = Params("/dev/shm/params")

  hpilot_functions = HpilotFunctions()
  theme_manager = ThemeManager()

  CP = None

  automatic_updates = params.get_bool("AutomaticUpdates")
  first_run = True
  time_validated = system_time_valid()

  pm = messaging.PubMaster(['hpilotPlan'])
  sm = messaging.SubMaster(['carState', 'controlsState', 'deviceState', 'hpilotCarControl', 'hpilotNavigation',
                            'hpilotPlan', 'liveLocationKalman', 'longitudinalPlan', 'modelV2', 'radarState'],
                           poll='modelV2', ignore_avg_freq=['radarState'])

  while True:
    sm.update()

    deviceState = sm['deviceState']
    started = deviceState.started

    if started:
      if CP is None:
        with car.CarParams.from_bytes(params.get("CarParams", block=True)) as msg:
          CP = msg
          hpilot_plannerd = HpilotPlannerd(CP)
          hpilot_plannerd.update_hpilot_params()

      if sm.updated['modelV2']:
        hpilot_plannerd.update(sm['carState'], sm['controlsState'], sm['hpilotCarControl'], sm['hpilotNavigation'],
                                  sm['liveLocationKalman'], sm['modelV2'], sm['radarState'])
        hpilot_plannerd.publish(sm, pm)

    if params_memory.get("ModelToDownload", encoding='utf-8') is not None and github_pinged():
      download_model()

    if params_memory.get_bool("HpilotTogglesUpdated"):
      automatic_updates = params.get_bool("AutomaticUpdates")

      if not params.get_bool("ModelSelector"):
        params.put("Model", DEFAULT_MODEL)
        params.put("ModelName", DEFAULT_MODEL_NAME)

      if started:
        hpilot_plannerd.update_hpilot_params()
      else:
        hpilot_functions.backup_toggles()

    if not time_validated:
      time_validated = system_time_valid()
      if not time_validated:
        continue

    if datetime.datetime.now().second == 0 or first_run or params_memory.get_bool("ManualUpdateInitiated"):
      if not started:
        time_checks(automatic_updates, deviceState, params)

      theme_manager.update_holiday()

      first_run = False

    time.sleep(DT_MDL)

def main():
  hpilot_thread()

if __name__ == "__main__":
  main()
