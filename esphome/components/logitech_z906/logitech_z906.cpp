#include "logitech_z906.h"

#ifdef USE_ESP32_FRAMEWORK_ARDUINO

#include "esphome/core/log.h"

// based heavily on https://github.com/zarpli/Logitech-Z906

namespace esphome {

namespace logitech_z906 {

media_player::MediaPlayerTraits LogitechZ906MediaPlayer::get_traits() {
  auto traits = media_player::MediaPlayerTraits();
  traits.set_supports_select_source(true);
  traits.set_supports_select_sound_mode(true);
  traits.set_supports_volume_mute(true);
  traits.set_supports_volume_set(true);
  traits.set_supports_volume_step(true);
  traits.set_supports_turn_off(true);
  traits.set_supports_turn_on(true);
  return traits;
};

media_player::MediaPlayerTraits SurroundSpeaker::get_traits() {
  auto traits = media_player::MediaPlayerTraits();
  traits.set_supports_volume_set(true);
  traits.set_supports_volume_step(true);
  return traits;
}

void LogitechZ906MediaPlayer::control(const media_player::MediaPlayerCall &call) {
  if (call.get_command().has_value()) {
    switch (call.get_command().value()) {
      case media_player::MEDIA_PLAYER_COMMAND_TURN_ON:
          this->turn_on_();
          break;
      case media_player::MEDIA_PLAYER_COMMAND_TURN_OFF:
          this->turn_off_();
          break;
      case media_player::MEDIA_PLAYER_COMMAND_MUTE:
          this->mute_();
        break;
      case media_player::MEDIA_PLAYER_COMMAND_UNMUTE:
          this->unmute_();
        break;
      case media_player::MEDIA_PLAYER_COMMAND_VOLUME_UP: {
        this->increase_volume_();
        this->unmute_();
        break;
      }
      case media_player::MEDIA_PLAYER_COMMAND_VOLUME_DOWN: {
        this->decrease_volume_();
        this->unmute_();
        break;
      }
      case media_player::MEDIA_PLAYER_COMMAND_VOLUME_SET: {
        float new_volume = call.get_volume().value();
        this->set_volume_(new_volume);
        this->unmute_();
        break;
      }
      case media_player::MEDIA_PLAYER_COMMAND_SELECT_SOURCE: {
        uint8_t source = source_name_to_cmd_.at(call.get_source().value().c_str());
        this->select_source_(source);
        update(); // the amp stores mode on a per-input basis
        // auto retrieve it from the amp when switching inputs so it gets reported to home assistant
        break;
      }
      case media_player::MEDIA_PLAYER_COMMAND_SELECT_SOUND_MODE: {
        uint8_t sound_mode = sound_mode_name_to_cmd_.at(call.get_sound_mode().value().c_str());
        this->select_sound_mode_(sound_mode);
        break;
      }
      default: {
        ESP_LOGW(TAG, "Unhandled command ID: %i", call.get_command().value());
      }
    }
  }
  this->publish_state();
}

void SurroundSpeaker::control(const media_player::MediaPlayerCall &call) {
  if (call.get_command().has_value()) {
    switch (call.get_command().value()) {
      case media_player::MEDIA_PLAYER_COMMAND_VOLUME_UP: {
        this->parent->increase_volume_(this->levelType);
        this->volume = remap<float, uint8_t>(this->parent->volumes[this->levelType], 0, 59, 0.0f, 1.0f);
        break;
      }
      case media_player::MEDIA_PLAYER_COMMAND_VOLUME_DOWN: {
        this->parent->decrease_volume_(this->levelType);
        this->volume = remap<float, uint8_t>(this->parent->volumes[this->levelType], 0, 59, 0.0f, 1.0f);
        break;
      }
      case media_player::MEDIA_PLAYER_COMMAND_VOLUME_SET: {
        float new_volume = call.get_volume().value();
        this->parent->set_volume_(this->levelType, new_volume);
        this->volume = new_volume;
        break;
      }
      default: {
        ESP_LOGW(TAG, "Unhandled command ID: %i", call.get_command().value());
      }
    }
  }
  this->publish_state();
  this->parent->publish_state();
}

void LogitechZ906MediaPlayer::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Logitech Z906");
  update();
  this->publish_state();
}

void SurroundSpeaker::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Logitech Z906 Surround Speaker");
  this->volume = remap<float, uint8_t>(this->parent->volumes[this->levelType], 0, 59, 0.0f, 1.0f);
  this->publish_state();
}

// Longitudinal Redundancy Check {-1,-1}
uint8_t LogitechZ906MediaPlayer::LRC(uint8_t *pData, uint8_t length) {
  uint8_t LRC = 0;

  for (int i = 1; i < length - 1; i++)
    LRC -= pData[i];

  return LRC;
}

void LogitechZ906MediaPlayer::clearBuffer() {
  flush();
  delay(5);
  uint8_t bytes = this->available();
  //ESP_LOGD(TAG, "%i bytes left over in buffer that will be cleared", bytes);
  for (int i = 0; i < bytes; i++) {
    read();
  }
}

int LogitechZ906MediaPlayer::update() {
  clearBuffer();
  this->write_byte(GET_STATUS);

  unsigned long currentMillis = millis();

  //ESP_LOGD(TAG, "Getting status, waiting for %i bytes (minimum size) to be available", STATUS_TOTAL_LENGTH);

  // Determine payload size and total required buffer size
  while (this->available() < STATUS_TOTAL_LENGTH) {
    if (millis() - currentMillis > SERIAL_TIME_OUT) {
      ESP_LOGW(TAG, "Timed out communicating with speakers, only %i bytes available", this->available());
      return 0;
    }
  }

  //ESP_LOGD(TAG, "Reading those bytes into memory");
  for (int i = 0; i < STATUS_TOTAL_LENGTH; i++) {
    status[i] = this->read();
  }

  uint8_t payload_len = status[STATUS_LENGTH]; //size of payload
  /**
   * total size is payload + 4 bytes
   * payload is always at least 19, so minimum is 23
   * when payload is larger than 19 read the additional bytes into the buffer
  **/
  status_len = payload_len + 4; //Size of full status buffer
  uint8_t additional_bytes = STATUS_TOTAL_LENGTH - status_len;

  //ESP_LOGD(TAG, "Status says payload is %i bytes", payload_len);
  //ESP_LOGD(TAG, "Therefore, the total size is %i bytes, %i bytes more than the minimum already read", status_len, additional_bytes);

  if (additional_bytes > 0) {
    //ESP_LOGD(TAG, "Waiting for %i bytes to be available", additional_bytes);
    while (this->available() < additional_bytes) {
      if (millis() - currentMillis > SERIAL_TIME_OUT) {
        ESP_LOGW(TAG, "Timed out communicating with speakers, only %i bytes available", this->available());
        return 0;
      }
    }

    //ESP_LOGD(TAG, "Reading additional bytes");
      // Read payload + checksum
      for(int i = 0; i <= additional_bytes; i++) {
        status[i + STATUS_LENGTH + 1] = this->read();
      }
  }

//  char buffer[3 * status_len + 1]; // Space for "XX " per byte, plus null terminator
//  char *p = buffer;
//  for (int i = 0; i < status_len; i++) {
//      p += sprintf(p, "%02X ", status[i]);
//  }
//  *(p - 1) = '\0'; // Replace trailing space with null terminator
//  ESP_LOGD(TAG, "Buffer: %s", buffer);

//  char vol[3]; // "XX\n\0"
//  sprintf(vol, "%02X", status[MAIN_LEVEL]);
//  ESP_LOGD(TAG, "Volume: %s", vol);

  uint8_t st_checksum_idx = status_len - 1;

  if (status[STATUS_STX] != EXP_STX) {
    char status_stx_str[3]; // "XX\0"
    sprintf(status_stx_str, "%02X", status[STATUS_STX]);
    char stx_str[3]; // "XX\0"
    sprintf(stx_str, "%02X", EXP_STX);
    ESP_LOGW(TAG, "STX error. Expected %s but got %s", stx_str, status_stx_str);
    return 1;
  }
  if (status[STATUS_MODEL] != EXP_MODEL_STATUS) {
    ESP_LOGW(TAG, "Model doesn't match expected value");
    return 1;
  }
  uint8_t lrc = LRC(status, STATUS_TOTAL_LENGTH);

  if (status[st_checksum_idx] != lrc) {
    char status_checksum_string[3]; // "XX\0"
    sprintf(status_checksum_string, "%02X", status[st_checksum_idx]);

    char lrc_string[3]; // "XX\0"
    sprintf(lrc_string, "%02X", lrc);

    ESP_LOGW(TAG, "Checksum failure. Expected %s but got %s", status_checksum_string, lrc_string);
    return 1;
   }

  this->volumes[MAIN_LEVEL] = status[STATUS_MAIN_LEVEL];
  this->volumes[REAR_LEVEL] = status[STATUS_REAR_LEVEL];
  this->volumes[CENTER_LEVEL] = status[STATUS_CENTER_LEVEL];
  this->volumes[SUB_LEVEL] = status[STATUS_SUB_LEVEL];

  uint8_t current_input = status[STATUS_CURRENT_INPUT];
  char input_str[3]; // "XX\n\0"
  sprintf(input_str, "%02X", current_input);
  ESP_LOGD(TAG, "Got input: %s", input_str);

  this->source = input_to_name_.at(current_input);
  uint8_t current_input_fx_status = input_to_status_.at(current_input);

  char mode_status_str[3]; // "XX\n\0"
  sprintf(mode_status_str, "%02X", current_input_fx_status);

  ESP_LOGD(TAG, "Dynamically selecting fx %s based on input of %s", mode_status_str, input_str);

  char mode_str[3]; // "XX\n\0"
  sprintf(mode_str, "%02X", status[current_input_fx_status]);
  ESP_LOGD(TAG, "Got mode status: %s", mode_str);

  std::string sm =  effect_status_to_name_.at(status[current_input_fx_status]);
  ESP_LOGD(TAG, "Got sound mode: %s", sm.c_str());
  this->sound_mode = sm;

  this->volume = remap<float, uint8_t>(this->volumes[MAIN_LEVEL], 0, 43, 0.0f, 1.0f);

  return 0;
}

int LogitechZ906MediaPlayer::request(uint8_t cmd) {
  if (update() == 0) {
    if (cmd == VERSION)
      return status[STATUS_VER_C] + 10 * status[STATUS_VER_B] + 100 * status[STATUS_VER_A];
    if (cmd == CURRENT_INPUT)
      return status[STATUS_CURRENT_INPUT] + 1;

    return status[cmd];
  }
  return 1;
}

int LogitechZ906MediaPlayer::cmd(uint8_t cmd) {
  this->write_byte(cmd);

  unsigned long currentMillis = millis();

  while (this->available() == 0) {
    if (millis() - currentMillis > SERIAL_TIME_OUT) {
      ESP_LOGW(TAG, "Timed out communicating with speakers");
      return 0;
    }
  }

  return read();
}

int LogitechZ906MediaPlayer::cmd(uint8_t cmd_a, uint8_t cmd_b) {
  if (update() == 0) {
    status[cmd_a] = cmd_b;

    status[STATUS_CHECKSUM] = LRC(status, STATUS_TOTAL_LENGTH);

    this->write_array(status, (size_t) STATUS_TOTAL_LENGTH);

    unsigned long currentMillis = millis();

    while (this->available() < ACK_TOTAL_LENGTH) {
      if (millis() - currentMillis > SERIAL_TIME_OUT) {
        ESP_LOGW(TAG, "Timed out communicating with speakers");
        return 0;
      }
    }

    uint8_t resp[ACK_TOTAL_LENGTH];
    return this->read_array(resp, (size_t) ACK_TOTAL_LENGTH);
  } else {
    return 1;
  }
}

uint8_t* LogitechZ906MediaPlayer::get_status() {
  update();

  return status;
}

uint8_t LogitechZ906MediaPlayer::main_sensor() {

  this->write_byte(GET_TEMP);

  unsigned long currentMillis = millis();

  while (this->available() < TEMP_TOTAL_LENGTH)
    if (millis() - currentMillis > SERIAL_TIME_OUT)
      return 0;

  uint8_t temp[TEMP_TOTAL_LENGTH];

  this->read_array(temp, (size_t) TEMP_TOTAL_LENGTH);

  if (temp[2] != EXP_MODEL_TEMP)
    return 0;

  return temp[7];
}

void LogitechZ906MediaPlayer::dump_config() {
  ESP_LOGCONFIG(TAG, "Logitech Z906:");
  if (this->is_failed()) {
    ESP_LOGCONFIG(TAG, "  Failed to initialize!");
    return;
  }
  ESP_LOGCONFIG(TAG, "  UART: %s", this->uart_name_.c_str());
}

void LogitechZ906MediaPlayer::mute_() {
  this->cmd(MUTE_ON);
  this->muted = true;
}
void LogitechZ906MediaPlayer::unmute_() {
  this->cmd(MUTE_OFF);
  this->muted = false;
}

void LogitechZ906MediaPlayer::turn_on_() {
  this->cmd(PWM_ON);
  this->state = media_player::MEDIA_PLAYER_STATE_ON;

  this->rear_speaker->state = media_player::MEDIA_PLAYER_STATE_ON;
  this->center_speaker->state = media_player::MEDIA_PLAYER_STATE_ON;
  this->sub_speaker->state = media_player::MEDIA_PLAYER_STATE_ON;

  this->rear_speaker->publish_state();
  this->center_speaker->publish_state();
  this->sub_speaker->publish_state();
}

void LogitechZ906MediaPlayer::turn_off_() {
  this->cmd(PWM_OFF);
  this->state = media_player::MEDIA_PLAYER_STATE_OFF;

  this->rear_speaker->state = media_player::MEDIA_PLAYER_STATE_OFF;
  this->center_speaker->state = media_player::MEDIA_PLAYER_STATE_OFF;
  this->sub_speaker->state = media_player::MEDIA_PLAYER_STATE_OFF;

  this->rear_speaker->publish_state();
  this->center_speaker->publish_state();
  this->sub_speaker->publish_state();
}

void LogitechZ906MediaPlayer::select_source_(uint8_t cmd) {
  this->cmd(cmd);
  this->source = source_cmd_to_name_.at(cmd);
}

void LogitechZ906MediaPlayer::select_sound_mode_(uint8_t cmd) {
  this->cmd(cmd);
  this->sound_mode = sound_mode_cmd_to_name_.at(cmd);
}

void LogitechZ906MediaPlayer::set_volume_(float volume, uint8_t levelType) {
  int intLevel;
  switch (levelType) {
    case MAIN_LEVEL:
      intLevel = remap<uint8_t, float>(volume, 0.0f, 1.0f, 0, 43);
      this->cmd(levelType, intLevel);
      this->volume = volume;
      this->volumes[levelType] = intLevel;
      break;
    case REAR_LEVEL:
    case CENTER_LEVEL:
    case SUB_LEVEL:
      intLevel = remap<uint8_t, float>(volume, 0.0f, 1.0f, 0, 59);
      this->cmd(levelType, intLevel);
      this->volumes[levelType] = intLevel;
      break;
  }
}

void LogitechZ906MediaPlayer::increase_volume_(uint8_t levelType) {
  switch (levelType) {
    case MAIN_LEVEL:
      this->cmd(LEVEL_MAIN_UP);
      if (volumes[levelType] < 43) volumes[levelType]++;
      this->volume = remap<float, uint8_t>(volumes[levelType], 0, 43, 0.0f, 1.0f);
      break;
    case REAR_LEVEL:
      this->cmd(LEVEL_REAR_UP);
      if (volumes[levelType] < 59) volumes[levelType]++;
      break;
    case CENTER_LEVEL:
      this->cmd(LEVEL_CENTER_UP);
      if (volumes[levelType] < 59) volumes[levelType]++;
      break;
    case SUB_LEVEL:
      this->cmd(LEVEL_SUB_UP);
      if (volumes[levelType] < 59) volumes[levelType]++;
      break;
  }
}

void LogitechZ906MediaPlayer::decrease_volume_(uint8_t levelType) {
  switch (levelType) {
    case MAIN_LEVEL:
      this->cmd(LEVEL_MAIN_DOWN);
      if (volumes[levelType] > 0) volumes[levelType]--;
      this->volume = remap<float, uint8_t>(volumes[levelType], 0, 43, 0.0f, 1.0f);
      break;
    case REAR_LEVEL:
      this->cmd(LEVEL_REAR_DOWN);
      if (volumes[levelType] > 0) volumes[levelType]--;
      break;
    case CENTER_LEVEL:
      this->cmd(LEVEL_CENTER_DOWN);
      if (volumes[levelType] > 0) volumes[levelType]--;
      break;
    case SUB_LEVEL:
      this->cmd(LEVEL_SUB_DOWN);
      if (volumes[levelType] > 0) volumes[levelType]--;
      break;
  }
}

void LogitechZ906MediaPlayer::set_up_surround_speakers() {
    ESP_LOGCONFIG(TAG, "Preparing Z906 surround speakers");
    this->rear_speaker = new SurroundSpeaker(REAR_LEVEL, this);
    this->center_speaker = new SurroundSpeaker(CENTER_LEVEL, this);
    this->sub_speaker = new SurroundSpeaker(SUB_LEVEL, this);

/*
    this->rear_speaker->add_on_state_callback([this]() {
      int intVolume = remap<uint8_t, float>(this->rear_speaker->volume, 0.0f, 1.0f, 0, 59);
      if (intVolume != this->volume_rear) {
        if (intVolume - this->volume_rear == 1) {
          this->increase_volume_(REAR_LEVEL);
        } else if (intVolume - this->volume_rear == -1) {
          this->decrease_volume_(REAR_LEVEL);
        } else {
          this->set_volume_(this->rear_speaker->volume, REAR_LEVEL);
        }
        this->rear_speaker->publish_state();
        this->publish_state();
      }
    });

    this->center_speaker->add_on_state_callback([this]() {
      int intVolume = remap<uint8_t, float>(this->center_speaker->volume, 0.0f, 1.0f, 0, 59);
      if (intVolume != this->volume_center) {
        if (intVolume - this->volume_rear == 1) {
          this->increase_volume_(CENTER_LEVEL);
        } else if (intVolume - this->volume_rear == -1) {
          this->decrease_volume_(CENTER_LEVEL);
        } else {
          this->set_volume_(this->center_speaker->volume, CENTER_LEVEL);
        }
        this->center_speaker->publish_state();
        this->publish_state();
      }
    });

    this->sub_speaker->add_on_state_callback([this]() {
      int intVolume = remap<uint8_t, float>(this->sub_speaker->volume, 0.0f, 1.0f, 0, 59);
      if (intVolume != this->volume_sub) {
        if (intVolume - this->volume_rear == 1) {
          this->increase_volume_(SUB_LEVEL);
        } else if (intVolume - this->volume_rear == -1) {
          this->decrease_volume_(SUB_LEVEL);
        } else {
          this->set_volume_(this->sub_speaker->volume, SUB_LEVEL);
        }
        this->sub_speaker->publish_state();
        this->publish_state();
      }
    });
    */

    App.register_component(this->rear_speaker);
    App.register_media_player(this->rear_speaker);
    App.register_component(this->center_speaker);
    App.register_media_player(this->center_speaker);
    App.register_component(this->sub_speaker);
    App.register_media_player(this->sub_speaker);
  }

}  // namespace logitech_z906
}  // namespace esphome

#endif  // USE_ESP32_FRAMEWORK_ARDUINO
