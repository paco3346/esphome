#pragma once

// based heavily on https://github.com/zarpli/Logitech-Z906

#ifdef USE_ESP32_FRAMEWORK_ARDUINO

#include "esphome/components/media_player/media_player.h"
#include "esphome/core/application.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/uart/uart.h"
#include <map>

namespace esphome {
namespace logitech_z906 {

static const char *const TAG = "audio";

#define SERIAL_TIME_OUT 1000

#define STATUS_TOTAL_LENGTH 0x17
#define ACK_TOTAL_LENGTH 0x05
#define TEMP_TOTAL_LENGTH 0x0A

// Single Commands

#define SELECT_INPUT_1 0x02
#define SELECT_INPUT_2 0x05
#define SELECT_INPUT_3 0x03
#define SELECT_INPUT_4 0x04
#define SELECT_INPUT_5 0x06
#define SELECT_INPUT_6 0x07

#define LEVEL_MAIN_UP 0x08
#define LEVEL_MAIN_DOWN 0x09
#define LEVEL_SUB_UP 0x0A
#define LEVEL_SUB_DOWN 0x0B
#define LEVEL_CENTER_UP 0x0C
#define LEVEL_CENTER_DOWN 0x0D
#define LEVEL_REAR_UP 0x0E
#define LEVEL_REAR_DOWN 0x0F

#define PWM_OFF 0x10
#define PWM_ON 0x11

#define SELECT_EFFECT_3D 0x14
#define SELECT_EFFECT_41 0x15
#define SELECT_EFFECT_21 0x16
#define SELECT_EFFECT_NO 0x35

#define EEPROM_SAVE 0x36

#define MUTE_ON 0x38
#define MUTE_OFF 0x39

#define BLOCK_INPUTS 0x22
#define RESET_PWR_UP_TIME 0x30
#define NO_BLOCK_INPUTS 0x33

// Double commands
#define MAIN_LEVEL 0x03
#define REAR_LEVEL 0x04
#define CENTER_LEVEL 0x05
#define SUB_LEVEL 0x06

// Requests
#define VERSION 0xF0
#define CURRENT_INPUT 0xF1
#define GET_INPUT_GAIN 0x2F
#define GET_TEMP 0x25
#define GET_PWR_UP_TIME 0x31
#define GET_STATUS 0x34

// MASK
#define EFFECT_3D 0x00
#define EFFECT_21 0x01
#define EFFECT_41 0x02
#define EFFECT_NO 0x03

#define INPUT_1 0x00
#define INPUT_2 0x01
#define INPUT_3 0x02
#define INPUT_4 0x03
#define INPUT_5 0x04
#define INPUT_6 0x06

#define SPK_NONE 0x00
#define SPK_ALL 0xFF
#define SPK_FR 0x01
#define SPK_FL 0x10
#define SPK_RR 0x02
#define SPK_RL 0x08
#define SPK_CENTER 0x04
#define SPK_SUB 0x20

#define INPUT_35MM "3.5 mm"
#define INPUT_RCA "RCA"
#define INPUT_OPTICAL1 "Optical 1"
#define INPUT_OPTICAL2 "Optical 2"
#define INPUT_COAXIAL "Coaxial"
#define INPUT_AUX "Aux"

#define SOUND_MODE_NONE "Default"
#define SOUND_MODE_3D "3D"
#define SOUND_MODE_21 "2.1"
#define SOUND_MODE_41 "4.1"

#define EXP_STX 0xAA
#define EXP_MODEL_STATUS 0x0A
#define EXP_MODEL_TEMP 0x0C

#define STATUS_STX 0x00
#define STATUS_MODEL 0x01
#define STATUS_LENGTH 0x02
#define STATUS_MAIN_LEVEL 0x03
#define STATUS_REAR_LEVEL 0x04
#define STATUS_CENTER_LEVEL 0x05
#define STATUS_SUB_LEVEL 0x06
#define STATUS_CURRENT_INPUT 0x07
#define STATUS_UNKNOWN 0x08
#define STATUS_FX_INPUT_4 0x09
#define STATUS_FX_INPUT_5 0x0A
#define STATUS_FX_INPUT_2 0x0B
#define STATUS_FX_INPUT_6 0x0C
#define STATUS_FX_INPUT_1 0x0D
#define STATUS_FX_INPUT_3 0x0E
#define STATUS_SPDIF_STATUS 0x0F
#define STATUS_SIGNAL_STATUS 0x10
#define STATUS_VER_A 0x11
#define STATUS_VER_B 0x12
#define STATUS_VER_C 0x13
#define STATUS_STBY 0x14
#define STATUS_AUTO_STBY 0x15
#define STATUS_CHECKSUM 0x16


const std::map<uint8_t, std::string> source_cmd_to_name_ {
  {SELECT_INPUT_1, INPUT_35MM},
  {SELECT_INPUT_2, INPUT_RCA},
  {SELECT_INPUT_3, INPUT_OPTICAL1},
  {SELECT_INPUT_4, INPUT_OPTICAL2},
  {SELECT_INPUT_5, INPUT_COAXIAL},
  {SELECT_INPUT_6, INPUT_AUX},
};

const std::map<uint8_t, std::string> input_to_name_ {
  {INPUT_1, INPUT_35MM},
  {INPUT_2, INPUT_RCA},
  {INPUT_3, INPUT_OPTICAL1},
  {INPUT_4, INPUT_OPTICAL2},
  {INPUT_5, INPUT_COAXIAL},
  {INPUT_6, INPUT_AUX},
};

const std::map<std::string, uint8_t> source_name_to_cmd_ {
  {INPUT_35MM, SELECT_INPUT_1},
  {INPUT_RCA, SELECT_INPUT_2},
  {INPUT_OPTICAL1, SELECT_INPUT_3},
  {INPUT_OPTICAL2, SELECT_INPUT_4},
  {INPUT_COAXIAL, SELECT_INPUT_5},
  {INPUT_AUX, SELECT_INPUT_6},
};

const std::map<uint8_t, std::string> sound_mode_cmd_to_name_ {
  {SELECT_EFFECT_NO, SOUND_MODE_NONE},
  {SELECT_EFFECT_3D, SOUND_MODE_3D},
  {SELECT_EFFECT_21, SOUND_MODE_21},
  {SELECT_EFFECT_41, SOUND_MODE_41}
};

const std::map<uint8_t, std::string> effect_status_to_name_ {
  {EFFECT_NO, SOUND_MODE_NONE},
  {EFFECT_3D, SOUND_MODE_3D},
  {EFFECT_21, SOUND_MODE_21},
  {EFFECT_41, SOUND_MODE_41}
};

const std::map<std::string, uint8_t> sound_mode_name_to_cmd_ {
  {SOUND_MODE_NONE, SELECT_EFFECT_NO},
  {SOUND_MODE_3D, SELECT_EFFECT_3D},
  {SOUND_MODE_41, SELECT_EFFECT_41},
  {SOUND_MODE_21, SELECT_EFFECT_21}
};

const std::map<uint8_t, uint8_t> input_to_status_ = {
  {INPUT_1, STATUS_FX_INPUT_1},
  {INPUT_2, STATUS_FX_INPUT_2},
  {INPUT_3, STATUS_FX_INPUT_3},
  {INPUT_4, STATUS_FX_INPUT_4},
  {INPUT_5, STATUS_FX_INPUT_5},
  {INPUT_6, STATUS_FX_INPUT_6},
};


class SurroundSpeaker;

class LogitechZ906MediaPlayer : public Component, public uart::UARTDevice, public media_player::MediaPlayer {
 public:
  LogitechZ906MediaPlayer(uart::UARTComponent *uart) : uart::UARTDevice(uart) {
    this->source_list.push_back(INPUT_35MM);
    this->source_list.push_back(INPUT_RCA);
    this->source_list.push_back(INPUT_OPTICAL1);
    this->source_list.push_back(INPUT_OPTICAL2);
    this->source_list.push_back(INPUT_COAXIAL);

    this->sound_mode_list.push_back(SOUND_MODE_NONE);
    this->sound_mode_list.push_back(SOUND_MODE_3D);
    this->sound_mode_list.push_back(SOUND_MODE_41);
    this->sound_mode_list.push_back(SOUND_MODE_21);

    volumes[MAIN_LEVEL] = 5;
    volumes[REAR_LEVEL] = 30;
    volumes[CENTER_LEVEL] = 30;
    volumes[SUB_LEVEL] = 30;
  }

  std::map<uint8_t, int> volumes;

  void setup() override;
  void set_up_surround_speakers();
  float get_setup_priority() const override { return esphome::setup_priority::LATE; }

  void dump_config() override;

  media_player::MediaPlayerTraits get_traits() override;
  void set_uart_name(const std::string &uart_name) { this->uart_name_ = uart_name; }

  int cmd(uint8_t);
  int cmd(uint8_t, uint8_t);
  int request(uint8_t);
  void print_status();

  void increase_volume_(uint8_t levelType = MAIN_LEVEL);
  void decrease_volume_(uint8_t levelType = MAIN_LEVEL);
  void set_volume_(float volume, uint8_t levelType = MAIN_LEVEL);

  uint8_t main_sensor();
  uint8_t* get_status();

 protected:
  void control(const media_player::MediaPlayerCall &call) override;

  std::string uart_name_;

  SurroundSpeaker* rear_speaker;
  SurroundSpeaker* center_speaker;
  SurroundSpeaker* sub_speaker;

  void mute_();
  void unmute_();
  void turn_on_();
  void turn_off_();
  void select_source_(uint8_t cmd);
  void select_sound_mode_(uint8_t cmd);

  void clearBuffer();
  int update();
  uint8_t LRC(uint8_t *, uint8_t);
  uint8_t status[STATUS_TOTAL_LENGTH];
  uint8_t status_len = 0;
};

class SurroundSpeaker : public Component, public media_player::MediaPlayer {
  public:

    SurroundSpeaker (uint8_t levelType, LogitechZ906MediaPlayer* parent) {
      this->levelType = levelType;
      this->parent = parent;

      switch (levelType) {
        case REAR_LEVEL:
          this->set_name(parent->get_name() + " Rear");
          break;
        case CENTER_LEVEL:
          this->set_name(parent->get_name() + " Center");
          break;
        case SUB_LEVEL:
          this->set_name(parent->get_name() + " Sub");
          break;
      }
    }

    void setup() override;
    float get_setup_priority() const override { return esphome::setup_priority::LATE; }
    media_player::MediaPlayerTraits get_traits() override;

  protected:
    void control(const media_player::MediaPlayerCall &call) override;
    uint8_t levelType;
    LogitechZ906MediaPlayer* parent;
};

}  // namespace logitech_z906
}  // namespace esphome

#endif  // USE_ESP32_FRAMEWORK_ARDUINO
