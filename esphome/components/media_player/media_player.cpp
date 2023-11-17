#include "media_player.h"

#include "esphome/core/log.h"

namespace esphome {
namespace media_player {

static const char *const TAG = "media_player";

const char *media_player_state_to_string(MediaPlayerState state) {
  switch (state) {
    case MEDIA_PLAYER_STATE_OFF:
      return "OFF";
    case MEDIA_PLAYER_STATE_ON:
      return "ON";
    case MEDIA_PLAYER_STATE_IDLE:
      return "IDLE";
    case MEDIA_PLAYER_STATE_PLAYING:
      return "PLAYING";
     case MEDIA_PLAYER_STATE_PAUSED:
      return "PAUSED";
     case MEDIA_PLAYER_STATE_STANDBY:
      return "STANDBY";
     case MEDIA_PLAYER_STATE_BUFFERING:
      return "BUFFERING";
    default:
      return "UNKNOWN";
  }
}

const char *media_player_command_to_string(MediaPlayerCommand command) {
  switch (command) {
    case MEDIA_PLAYER_COMMAND_PAUSE:
      return "PAUSE";
    case MEDIA_PLAYER_COMMAND_SEEK:
      return "SEEK";
    case MEDIA_PLAYER_COMMAND_VOLUME_SET:
      return "VOLUME_SET";
    case MEDIA_PLAYER_COMMAND_MUTE:
      return "MUTE";
    case MEDIA_PLAYER_COMMAND_UNMUTE:
      return "UNMUTE";
    case MEDIA_PLAYER_COMMAND_PREVIOUS_TRACK:
      return "PREVIOUS_TRACK";
    case MEDIA_PLAYER_COMMAND_NEXT_TRACK:
      return "NEXT_TRACK";
    case MEDIA_PLAYER_COMMAND_TURN_ON:
      return "TURN_ON";
    case MEDIA_PLAYER_COMMAND_TURN_OFF:
      return "TURN_OFF";
    case MEDIA_PLAYER_COMMAND_PLAY_MEDIA:
      return "PLAY_MEDIA";
    case MEDIA_PLAYER_COMMAND_VOLUME_UP:
      return "VOLUME_UP";
    case MEDIA_PLAYER_COMMAND_VOLUME_DOWN:
      return "VOLUME_DOWN";
    case MEDIA_PLAYER_COMMAND_SELECT_SOURCE:
      return "SELECT_SOURCE";
    case MEDIA_PLAYER_COMMAND_STOP:
      return "STOP";
    case MEDIA_PLAYER_COMMAND_CLEAR_PLAYLIST:
      return "CLEAR_PLAYLIST";
    case MEDIA_PLAYER_COMMAND_PLAY:
      return "PLAY";
    case MEDIA_PLAYER_COMMAND_SHUFFLE_SET:
      return "SHUFFLE_SET";
    case MEDIA_PLAYER_COMMAND_SELECT_SOUND_MODE:
      return "SELECT_SOUND_MODE";
    case MEDIA_PLAYER_COMMAND_REPEAT_SET:
      return "REPEAT_SET";
    default:
      return "UNKNOWN";
  }
}

void MediaPlayerCall::validate_() {
  if (this->media_url_.has_value()) {
    if (this->command_.has_value()) {
      ESP_LOGW(TAG, "MediaPlayerCall: Setting both command and media_url is not needed.");
      this->command_.reset();
    }
  }
  if (this->volume_.has_value()) {
    if (this->volume_.value() < 0.0f || this->volume_.value() > 1.0f) {
      ESP_LOGW(TAG, "MediaPlayerCall: Volume must be between 0.0 and 1.0.");
      this->volume_.reset();
    }
  }
}

void MediaPlayerCall::perform() {
  ESP_LOGD(TAG, "'%s' - Setting", this->parent_->get_name().c_str());
  this->validate_();
  if (this->command_.has_value()) {
    const char *command_s = media_player_command_to_string(this->command_.value());
    ESP_LOGD(TAG, "  Command: %s", command_s);
  }
  if (this->media_url_.has_value()) {
    ESP_LOGD(TAG, "  Media URL: %s", this->media_url_.value().c_str());
  }
  if (this->volume_.has_value()) {
    ESP_LOGD(TAG, "  Volume: %.2f", this->volume_.value());
  }
  if (this->source_.has_value()) {
    ESP_LOGD(TAG, "  Source: %s", this->source_.value().c_str());
  }
  if (this->sound_mode_.has_value()) {
    ESP_LOGD(TAG, "  Sound Mode: %s", this->sound_mode_.value().c_str());
  }
  this->parent_->control(*this);
}

MediaPlayerCall &MediaPlayerCall::set_command(MediaPlayerCommand command) {
  this->command_ = command;
  return *this;
}
MediaPlayerCall &MediaPlayerCall::set_command(optional<MediaPlayerCommand> command) {
  this->command_ = command;
  return *this;
}
MediaPlayerCall &MediaPlayerCall::set_command(const std::string &command) {
  if (str_equals_case_insensitive(command, "PLAY")) {
    this->set_command(MEDIA_PLAYER_COMMAND_PLAY);
  } else if (str_equals_case_insensitive(command, "PAUSE")) {
    this->set_command(MEDIA_PLAYER_COMMAND_PAUSE);
  } else if (str_equals_case_insensitive(command, "STOP")) {
    this->set_command(MEDIA_PLAYER_COMMAND_STOP);
  } else if (str_equals_case_insensitive(command, "MUTE")) {
    this->set_command(MEDIA_PLAYER_COMMAND_MUTE);
  } else if (str_equals_case_insensitive(command, "UNMUTE")) {
    this->set_command(MEDIA_PLAYER_COMMAND_UNMUTE);
  } else {
    ESP_LOGW(TAG, "'%s' - Unrecognized command %s", this->parent_->get_name().c_str(), command.c_str());
  }
  return *this;
}

MediaPlayerCall &MediaPlayerCall::set_media_url(const std::string &media_url) {
  this->media_url_ = media_url;
  return *this;
}

MediaPlayerCall &MediaPlayerCall::seek_position(float position) {
  //TODO: handle seek call
  return *this;
}

MediaPlayerCall &MediaPlayerCall::set_volume(float volume) {
  this->volume_ = volume;
  return *this;
}

MediaPlayerCall &MediaPlayerCall::set_muted(bool muted) {
  this->muted_ = muted;
  return *this;
}

MediaPlayerCall &MediaPlayerCall::set_shuffle(bool shuffle_set) {
  this->shuffle_set_ = shuffle_set;
  return *this;
}

MediaPlayerCall &MediaPlayerCall::set_source(const std::string &source) {
  this->source_ = source;
  return *this;
}

MediaPlayerCall &MediaPlayerCall::set_sound_mode(const std::string &sound_mode) {
  this->sound_mode_ = sound_mode;
  return *this;
}

MediaPlayerCall &MediaPlayerCall::set_repeat_mode(optional<MediaPlayerRepeatMode> repeat_mode) {
  this->repeat_mode_ = repeat_mode;
  return *this;
}

void MediaPlayer::add_on_state_callback(std::function<void()> &&callback) {
  this->state_callback_.add(std::move(callback));
}

void MediaPlayer::publish_state() { this->state_callback_.call(); }

}  // namespace media_player
}  // namespace esphome
