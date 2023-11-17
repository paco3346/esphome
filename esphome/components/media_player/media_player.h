#pragma once

#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace media_player {

enum MediaPlayerState : uint8_t {
  MEDIA_PLAYER_STATE_OFF = 0,
  MEDIA_PLAYER_STATE_ON = 1,
  MEDIA_PLAYER_STATE_IDLE = 2,
  MEDIA_PLAYER_STATE_PLAYING = 3,
  MEDIA_PLAYER_STATE_PAUSED = 4,
  MEDIA_PLAYER_STATE_STANDBY = 5,
  MEDIA_PLAYER_STATE_BUFFERING = 6,
};
const char *media_player_state_to_string(MediaPlayerState state);

enum MediaPlayerCommand : uint8_t {
  MEDIA_PLAYER_COMMAND_PAUSE = 0,
  MEDIA_PLAYER_COMMAND_SEEK = 1,
  MEDIA_PLAYER_COMMAND_VOLUME_SET = 2,
  MEDIA_PLAYER_COMMAND_MUTE = 3,
  MEDIA_PLAYER_COMMAND_UNMUTE = 4,
  MEDIA_PLAYER_COMMAND_PREVIOUS_TRACK = 5,
  MEDIA_PLAYER_COMMAND_NEXT_TRACK = 6,
  MEDIA_PLAYER_COMMAND_TURN_ON = 7,
  MEDIA_PLAYER_COMMAND_TURN_OFF = 8,
  MEDIA_PLAYER_COMMAND_PLAY_MEDIA = 9,
  MEDIA_PLAYER_COMMAND_VOLUME_UP = 10,
  MEDIA_PLAYER_COMMAND_VOLUME_DOWN = 11,
  MEDIA_PLAYER_COMMAND_SELECT_SOURCE = 12,
  MEDIA_PLAYER_COMMAND_STOP = 13,
  MEDIA_PLAYER_COMMAND_CLEAR_PLAYLIST = 14,
  MEDIA_PLAYER_COMMAND_PLAY = 15,
  MEDIA_PLAYER_COMMAND_SHUFFLE_SET = 16,
  MEDIA_PLAYER_COMMAND_SELECT_SOUND_MODE = 17,
  MEDIA_PLAYER_COMMAND_REPEAT_SET = 18,
};

const char *media_player_command_to_string(MediaPlayerCommand command);

enum MediaPlayerRepeatMode : uint8_t {
  MEDIA_PLAYER_REPEAT_MODE_OFF = 0,
  MEDIA_PLAYER_REPEAT_MODE_ONE = 1,
  MEDIA_PLAYER_REPEAT_MODE_ALL = 2,
};

class MediaPlayer;

class MediaPlayerTraits {
 public:
  MediaPlayerTraits() = default;

  void set_supports_pause(bool supports_pause) { this->supports_pause_ = supports_pause; }

  bool get_supports_pause() const { return this->supports_pause_; }

  void set_supports_seek(bool supports_seek) { this->supports_seek_ = supports_seek; }

  bool get_supports_seek() const { return this->supports_seek_; }

  void set_supports_volume_set(bool supports_volume_set) { this->supports_volume_set_ = supports_volume_set; }

  bool get_supports_volume_set() const { return this->supports_volume_set_; }

  void set_supports_volume_mute(bool supports_volume_mute) { this->supports_volume_mute_ = supports_volume_mute; }

  bool get_supports_volume_mute() const { return this->supports_volume_mute_; }

  void set_supports_previous_track(bool supports_previous_track) { this->supports_previous_track_ = supports_previous_track; }

  bool get_supports_previous_track() const { return this->supports_previous_track_; }

  void set_supports_next_track(bool supports_next_track) { this->supports_next_track_ = supports_next_track; }

  bool get_supports_next_track() const { return this->supports_next_track_; }

  void set_supports_turn_on(bool supports_turn_on) { this->supports_turn_on_ = supports_turn_on; }

  bool get_supports_turn_on() const { return this->supports_turn_on_; }

  void set_supports_turn_off(bool supports_turn_off) { this->supports_turn_off_ = supports_turn_off; }

  bool get_supports_turn_off() const { return this->supports_turn_off_; }

  void set_supports_play_media(bool supports_play_media) { this->supports_play_media_ = supports_play_media; }

  bool get_supports_play_media() const { return this->supports_play_media_; }

  void set_supports_volume_step(bool supports_volume_step) { this->supports_volume_step_ = supports_volume_step; }

  bool get_supports_volume_step() const { return this->supports_volume_step_; }

  void set_supports_select_source(bool supports_select_source) { this->supports_select_source_ = supports_select_source; }

  bool get_supports_select_source() const { return this->supports_select_source_; }

  void set_supports_stop(bool supports_stop) { this->supports_stop_ = supports_stop; }

  bool get_supports_stop() const { return this->supports_stop_; }

  void set_supports_clear_playlist(bool supports_clear_playlist) { this->supports_clear_playlist_ = supports_clear_playlist; }

  bool get_supports_clear_playlist() const { return this->supports_clear_playlist_; }

  void set_supports_play(bool supports_play) { this->supports_play_ = supports_play; }

  bool get_supports_play() const { return this->supports_play_; }

  void set_supports_shuffle_set(bool supports_shuffle_set) { this->supports_shuffle_set_ = supports_shuffle_set; }

  bool get_supports_shuffle_set() const { return this->supports_shuffle_set_; }

  void set_supports_select_sound_mode(bool supports_select_sound_mode) { this->supports_select_sound_mode_ = supports_select_sound_mode; }

  bool get_supports_select_sound_mode() const { return this->supports_select_sound_mode_; }

  void set_supports_repeat_set(bool supports_repeat_set) { this->supports_repeat_set_ = supports_repeat_set; }

  bool get_supports_repeat_set() const { return this->supports_repeat_set_; }

 protected:
  bool supports_pause_{false};
  bool supports_seek_{false};
  bool supports_volume_set_{false};
  bool supports_volume_mute_{false};
  bool supports_previous_track_{false};
  bool supports_next_track_{false};
  bool supports_turn_on_{false};
  bool supports_turn_off_{false};
  bool supports_play_media_{false};
  bool supports_volume_step_{false};
  bool supports_select_source_{false};
  bool supports_stop_{false};
  bool supports_clear_playlist_{false};
  bool supports_play_{false};
  bool supports_shuffle_set_{false};
  bool supports_select_sound_mode_{false};
  bool supports_repeat_set_{false};
};

class MediaPlayerCall {
 public:
  MediaPlayerCall(MediaPlayer *parent) : parent_(parent) {}

  MediaPlayerCall &set_command(MediaPlayerCommand command);
  MediaPlayerCall &set_command(optional<MediaPlayerCommand> command);
  MediaPlayerCall &set_command(const std::string &command);

  MediaPlayerCall &set_media_url(const std::string &url);

  MediaPlayerCall &seek_position(float volume);
  MediaPlayerCall &set_volume(float volume);
  MediaPlayerCall &set_muted(bool muted);
  MediaPlayerCall &set_shuffle(bool shuffle_set);
  MediaPlayerCall &set_source(const std::string &source);
  MediaPlayerCall &set_sound_mode(const std::string &sound_mode);
  MediaPlayerCall &set_repeat_mode(optional<MediaPlayerRepeatMode> repeat_mode);

  void perform();

  const optional<MediaPlayerCommand> &get_command() const { return command_; }
  const optional<std::string> &get_media_url() const { return media_url_; }
  const optional<float> &get_volume() const { return volume_; }
  const optional<bool> &get_muted() const { return muted_; }
  const optional<std::string> &get_source() const { return source_; }
  const optional<std::string> &get_sound_mode() const { return sound_mode_; }
  const optional<MediaPlayerRepeatMode> &get_repeat_mode() const { return repeat_mode_; }

 protected:
  void validate_();
  MediaPlayer *const parent_;
  optional<MediaPlayerCommand> command_;
  optional<std::string> media_url_;
  optional<float> volume_;
  optional<bool> muted_;
  optional<std::string> source_;
  optional<std::string> sound_mode_;
  optional<MediaPlayerRepeatMode> repeat_mode_;
  optional<bool> shuffle_set_;
};

class MediaPlayer : public EntityBase {
 public:
  MediaPlayerState state{MEDIA_PLAYER_STATE_OFF};
  float volume{1.0f};
  bool muted{false};
  std::string source{};
  std::string sound_mode{};
  MediaPlayerRepeatMode repeat_mode{MEDIA_PLAYER_REPEAT_MODE_OFF};
  bool shuffle_set{false};
  std::vector<std::string> source_list{};
  std::vector<std::string> sound_mode_list{};

  MediaPlayerCall make_call() { return MediaPlayerCall(this); }

  void publish_state();

  void add_on_state_callback(std::function<void()> &&callback);

  virtual MediaPlayerTraits get_traits() = 0;

 protected:
  friend MediaPlayerCall;

  virtual void control(const MediaPlayerCall &call) = 0;

  CallbackManager<void()> state_callback_{};
};

}  // namespace media_player
}  // namespace esphome
