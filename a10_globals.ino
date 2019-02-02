/*******************************************************************************
#                                                                              #
#     An alternative firmware for Xiaomi Desk Lamp (Yeelight)                  #
#                                                                              #
#                                                                              #
#      Copyright (C) 2018 Tom Stöveken                                         #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
********************************************************************************/

#include <ArduinoJson.h>
#include <deque>

//determine array length
#define LENGTH_OF(x) (sizeof(x)/sizeof(x[0]))

/* current state of the device */
enum STATES {UNDEF=0, CONSTANTCOLOR, NOTIFICATION, BOOTUP, FLASH, LIGHTSOFF, RESET_CONFIGURATION};
struct {
  STATES state;
  String human_readable_string;
  String state_as_string;
} state_map[] = {
  { UNDEF, "Undefined State", "undef" },
  { CONSTANTCOLOR,"Static Color Mode", "constantcolor" },
  { NOTIFICATION,"Notification mode", "notification" },
  { BOOTUP, "Booting up...", "bootup" },
  { FLASH, "Flashing Firmware...", "flash" },
  { LIGHTSOFF, "Lights switched off", "lightsoff" },
  { RESET_CONFIGURATION, "Resetting all configurations", "reset_configuration" }
};
STATES state = UNDEF;
STATES old_state = UNDEF;

/* Variables for the warm and cold white LEDs */
float g_brightness, g_ratio, old_g_brightness, old_g_ratio;

/* Hostname, this name will show up in DHCP requests */
String g_hostname;

bool g_send_WLAN_keep_alive_packet;

/* Storage for most recent logging messages */
std::deque<String> log_messages;

#define LOG_LENGTH 100

/* logging messages, maintain length of entries */
void Log(String text) {
  log_messages.push_back(text);
  log_messages.pop_front();

  Serial.println(text);
}

/* only allow to write to flash if commando was set */
bool enableUpdates = false;
