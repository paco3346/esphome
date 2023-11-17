#pragma once

#include "esphome/core/automation.h"
#include "logitech_z906.h"

namespace esphome {

namespace logitech_z906 {

class VolumeChangeRearTrigger : public Trigger<> {
  public:
    VolumeChangeRearTrigger(LogitechZ906MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->volumes[REAR_LEVEL] != this->last_volume_) {
          this->trigger();
        }
        this->last_volume_ = player->volumes[REAR_LEVEL];
      });
    }

  protected:
    int last_volume_ = 0;
};

class VolumeChangeCenterTrigger : public Trigger<> {
  public:
    VolumeChangeCenterTrigger(LogitechZ906MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->volumes[CENTER_LEVEL] != this->last_volume_) {
          this->trigger();
        }
        this->last_volume_ = player->volumes[CENTER_LEVEL];
      });
    }

  protected:
    int last_volume_ = 0;
};

class VolumeChangeSubTrigger : public Trigger<> {
  public:
    VolumeChangeSubTrigger(LogitechZ906MediaPlayer *player) {
      player->add_on_state_callback([this, player]() {
        if (player->volumes[SUB_LEVEL] != this->last_volume_) {
          this->trigger();
        }
        this->last_volume_ = player->volumes[SUB_LEVEL] ;
      });
    }

  protected:
    int last_volume_ = 0;
};

}  // namespace logitech_z906
}  // namespace esphome
