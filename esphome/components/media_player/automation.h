#pragma once

#include "esphome/core/automation.h"
#include "media_player.h"

namespace esphome {

namespace media_player {

#define MEDIA_PLAYER_SIMPLE_COMMAND_ACTION(ACTION_CLASS, ACTION_COMMAND) \
  template<typename... Ts> class ACTION_CLASS : public Action<Ts...>, public Parented<MediaPlayer> { \
    void play(Ts... x) override { \
      this->parent_->make_call().set_command(MediaPlayerCommand::MEDIA_PLAYER_COMMAND_##ACTION_COMMAND).perform(); \
    } \
  };

#define MEDIA_PLAYER_SIMPLE_STATE_TRIGGER(TRIGGER_CLASS, TRIGGER_STATE) \
  class TRIGGER_CLASS : public Trigger<> { \
   public: \
    explicit TRIGGER_CLASS(MediaPlayer *player) { \
      player->add_on_state_callback([this, player]() { \
        if (player->state == MediaPlayerState::MEDIA_PLAYER_STATE_##TRIGGER_STATE) \
          this->trigger(); \
      }); \
    } \
  };

MEDIA_PLAYER_SIMPLE_COMMAND_ACTION(PlayAction, PLAY)
MEDIA_PLAYER_SIMPLE_COMMAND_ACTION(PauseAction, PAUSE)
MEDIA_PLAYER_SIMPLE_COMMAND_ACTION(StopAction, STOP)
MEDIA_PLAYER_SIMPLE_COMMAND_ACTION(VolumeUpAction, VOLUME_UP)
MEDIA_PLAYER_SIMPLE_COMMAND_ACTION(VolumeDownAction, VOLUME_DOWN)
MEDIA_PLAYER_SIMPLE_COMMAND_ACTION(PowerOnAction, TURN_ON)
MEDIA_PLAYER_SIMPLE_COMMAND_ACTION(PowerOffAction, TURN_OFF)

template<typename... Ts> class PlayMediaAction : public Action<Ts...>, public Parented<MediaPlayer> {
  TEMPLATABLE_VALUE(std::string, media_url)
  void play(Ts... x) override { this->parent_->make_call().set_media_url(this->media_url_.value(x...)).perform(); }
};

template<typename... Ts> class VolumeSetAction : public Action<Ts...>, public Parented<MediaPlayer> {
  TEMPLATABLE_VALUE(float, volume)
  void play(Ts... x) override { this->parent_->make_call().set_volume(this->volume_.value(x...)).perform(); }
};

class StateTrigger : public Trigger<> {
 public:
  explicit StateTrigger(MediaPlayer *player) {
    player->add_on_state_callback([this]() { this->trigger(); });
  }
};

MEDIA_PLAYER_SIMPLE_STATE_TRIGGER(IdleTrigger, IDLE)
MEDIA_PLAYER_SIMPLE_STATE_TRIGGER(PlayTrigger, PLAYING)
MEDIA_PLAYER_SIMPLE_STATE_TRIGGER(PauseTrigger, PAUSED)

class PowerOnTrigger : public Trigger<> {
  public:
    PowerOnTrigger(MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->state != this->last_state_ && player->state == MEDIA_PLAYER_STATE_ON) {
          this->trigger();
        }
        this->last_state_ = player->state;
      });
    }

  protected:
    MediaPlayerState last_state_ = MEDIA_PLAYER_STATE_OFF;
};

class PowerOffTrigger : public Trigger<> {
  public:
    PowerOffTrigger(MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->state != this->last_state_ && player->state == MEDIA_PLAYER_STATE_OFF) {
          this->trigger();
        }
        this->last_state_ = player->state;
      });
    }

  protected:
    MediaPlayerState last_state_ = MEDIA_PLAYER_STATE_OFF;
};

class VolumeChangeTrigger : public Trigger<> {
  public:
    VolumeChangeTrigger(MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->volume != this->last_volume_) {
          this->trigger();
        }
        this->last_volume_ = player->volume;
      });
    }

  protected:
    float last_volume_ = 0.0;
};

class MutedTrigger : public Trigger<> {
  public:
    MutedTrigger(MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->muted != this->last_state_) {
          this->trigger();
        }
        this->last_state_ = player->muted;
      });
    }

  protected:
    bool last_state_ = false;
};

class UnmutedTrigger : public Trigger<> {
  public:
    UnmutedTrigger(MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->muted == this->last_state_) {
          this->trigger();
        }
        this->last_state_ = not player->muted;
      });
    }

  protected:
    bool last_state_ = true;
};

class SourceTrigger : public Trigger<> {
  public:
    SourceTrigger(MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->source != this->last_source_) {
          this->trigger();
        }
        this->last_source_ = player->source;
      });
    }

  protected:
    std::string last_source_ = "";
};

class SoundModeTrigger : public Trigger<> {
  public:
    SoundModeTrigger(MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->sound_mode != this->last_mode_) {
          this->trigger();
        }
        this->last_mode_ = player->sound_mode;
      });
    }

  protected:
    std::string last_mode_ = "";
};

class SourceListTrigger : public Trigger<> {
  public:
    SourceListTrigger(MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->source_list != this->last_list_) {
          this->trigger();
        }
        this->last_list_ = player->source_list;
      });
    }

  protected:
    std::vector<std::string> last_list_;
};

template<typename... Ts> class IsIdleCondition : public Condition<Ts...>, public Parented<MediaPlayer> {
 public:
  bool check(Ts... x) override { return this->parent_->state == MediaPlayerState::MEDIA_PLAYER_STATE_IDLE; }
};

template<typename... Ts> class IsPlayingCondition : public Condition<Ts...>, public Parented<MediaPlayer> {
 public:
  bool check(Ts... x) override { return this->parent_->state == MediaPlayerState::MEDIA_PLAYER_STATE_PLAYING; }
};

template<typename... Ts> class IsOnCondition : public Condition<Ts...>, public Parented<MediaPlayer> {
 public:
  bool check(Ts... x) override { return this->parent_->state == MediaPlayerState::MEDIA_PLAYER_STATE_ON; }
};

}  // namespace media_player
}  // namespace esphome
