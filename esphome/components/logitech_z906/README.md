Sample usage:

```yaml
uart:
  baud_rate: 57600
  parity: ODD
  stop_bits: 1
  tx_pin: { tx_pin }
  rx_pin: { rx_pin }
  data_bits: 8
  id: uart_1


media_player:
  - platform: logitech_z906
    id: speakers
    name: "Logitech Z906"
    uart_id: uart_1
```

Fully featured version with OLED display:

```yaml
esphome:
  name: logitech-z906
  includes:
    - logitech_z906_screen_backlight.h # see bottom of readme
    - logitech_z906_screen_timeout.h # see bottom of readme

globals:
   - id: smoothed_volume
     type: float
     restore_value: no
     initial_value: '0'

ethernet:
  type: RTL8201
  mdc_pin: GPIO16
  mdio_pin: GPIO17
  clk_mode: GPIO0_IN
  phy_addr: 0

i2c:
  sda: GPIO15
  scl: GPIO4
  frequency: 400kHz

font:
  - file: "gfonts://Open+Sans"
    id: open_sans
    size: 28
  - file: "gfonts://Open+Sans"
    id: open_sans_small
    size: 20
  - file: "gfonts://Open+Sans"
    id: open_sans_tiny
    size: 14

display:
  - platform: ssd1306_i2c
    id: oled_screen
    model: "SSD1306 128x64"
    contrast: 100%
    address: 0x3C
    update_interval: never
    pages:
      - id: scr_off
        lambda: |-
          it.line(0, 63, 0, 63);
      - id: scr_volume
        lambda: |-
          float vol = id(smoothed_volume);
          int targetVolume = id(speakers).volumes[MAIN_LEVEL];
          bool muted = id(speakers).muted;
          if (muted) {
            it.print(it.get_width()/2, it.get_height()/2, id(open_sans_small), TextAlign::CENTER, "Mute");
          } else {
            it.printf(it.get_width()/2, it.get_height()/2, id(open_sans), TextAlign::CENTER, "%i", targetVolume);
          }
          it.arc(63, 36, 33, 120, (120 + vol * 6.98));
      - id: scr_volume_rear
        lambda: |-
          int vol = id(speakers).volumes[REAR_LEVEL];
          it.print(it.get_width()/2, it.get_height()/2 - 2, id(open_sans_tiny), TextAlign::CENTER, "Rear");
          it.printf(it.get_width()/2, it.get_height()/2 + 18, id(open_sans_small), TextAlign::CENTER, "%+i", vol - 30);
          it.arc(63, 36, 33, 120, (120 + vol * 5));
      - id: scr_volume_center
        lambda: |-
          int vol = id(speakers).volumes[CENTER_LEVEL];
          it.print(it.get_width()/2, it.get_height()/2 - 2, id(open_sans_tiny), TextAlign::CENTER, "Center");
          it.printf(it.get_width()/2, it.get_height()/2 + 18, id(open_sans_small), TextAlign::CENTER, "%+i", vol - 30);
          it.arc(63, 36, 33, 120, (120 + vol * 5));
      - id: scr_volume_sub
        lambda: |-
          int vol = id(speakers).volumes[SUB_LEVEL];
          it.print(it.get_width()/2, it.get_height()/2 - 2, id(open_sans_tiny), TextAlign::CENTER, "Sub");
          it.printf(it.get_width()/2, it.get_height()/2 + 18, id(open_sans_small), TextAlign::CENTER, "%+i", vol - 30);
          it.arc(63, 36, 33, 120, (120 + vol * 5));
      - id: scr_source_and_mode
        lambda: |-
          it.printf(it.get_width()/2, it.get_height()/2 - 9, id(open_sans_small), TextAlign::CENTER, "%s", id(speakers).source.c_str());
          it.printf(it.get_width()/2, it.get_height()/2 + 9, id(open_sans_small), TextAlign::CENTER, "%s", id(speakers).sound_mode.c_str());

light:
  - platform: custom
    lambda: |-
      auto light_out = new LogitechZ906ScreenBacklight(id(oled_screen));
      App.register_component(light_out);
      return {light_out};
    lights: 
      - name: "Screen Brightness"

external_components:
  - source:
      type: local
      path: /config/esp_components
    components: [ logitech_z906, display, api, media_player, hdmi_cec ]

custom_component:
  - lambda: |-
      screen_timeout = new LogitechZ906ScreenTimeout(id(oled_screen), id(scr_off));
      return {screen_timeout};

uart:
  baud_rate: 57600
  parity: ODD
  stop_bits: 1
  tx_pin: GPIO12
  rx_pin: GPIO13
  data_bits: 8
  id: uart_1


media_player:
  - platform: logitech_z906
    id: speakers
    name: "Logitech Z906"
    uart_id: uart_1
    on_power_on:
      then:
        - lambda: |-
            id(oled_screen).turn_on();
            screen_timeout->auto_page(scr_volume);
    on_power_off:
      then:
        - lambda: |-
            id(oled_screen).turn_off();
    on_volume_change:
      then:
        - lambda: |-
            screen_timeout->auto_page(scr_volume, &id(smoothed_volume), id(speakers).volumes[MAIN_LEVEL]);
    on_volume_change_rear:
      then:
        - lambda: |-
            screen_timeout->auto_page(scr_volume_rear);
    on_volume_change_center:
      then:
        - lambda: |-
            screen_timeout->auto_page(scr_volume_center);
    on_volume_change_sub:
      then:
        - lambda: |-
            screen_timeout->auto_page(scr_volume_sub);
    on_muted:
      then:
        - lambda: |-
            screen_timeout->auto_page(scr_volume);
    on_unmuted:
      then:
        - lambda: |-
            screen_timeout->auto_page(scr_volume);
    on_source_changed:
      then:
        - lambda: |-
            screen_timeout->auto_page(scr_source_and_mode);
    on_sound_mode_changed:
      then:
        - lambda: |-
            screen_timeout->auto_page(scr_source_and_mode);
```

`logitech_z906_screen_backlight.h`
```cpp
#include "esphome.h"

class LogitechZ906ScreenBacklight : public Component, public LightOutput {
  private:
    ssd1306_base::SSD1306* oledScreen_;
  public:
    LogitechZ906ScreenBacklight(ssd1306_base::SSD1306 *oledScreen) {
      this->oledScreen_ = oledScreen;
    }
    
    LightTraits get_traits() override {
      auto traits = LightTraits();
      traits.set_supported_color_modes({ColorMode::BRIGHTNESS});
      return traits;
    }

    void write_state(LightState *state) override {
      float brightness;
      state->current_values_as_brightness(&brightness);
      this->oledScreen_->set_contrast(brightness);
    }
  };
```

`logitech_z906_screen_timeout.h`
```cpp
#include "esphome.h"

class LogitechZ906ScreenTimeout  : public Component {

  private:
    ssd1306_base::SSD1306* oledScreen;
    int delay = 5000;
    int volAnimationSteps = 5;
    int volAnimationTime = 300;
    DisplayPage* offPage;
  public:
    LogitechZ906ScreenTimeout(ssd1306_base::SSD1306 *oledScreen, DisplayPage *offPage) {
      this->oledScreen = oledScreen;
      this->offPage = offPage;
    }

    void set_delay(int delay) {
      this->delay = delay;
    }

    void auto_page(DisplayPage *page, float *smoothedVolume = nullptr, int targetVolume = 0) {
      cancel_timeout("autoOff");
      char timeoutName[10]; // autoVol00\0
      for (int i = 0; i < volAnimationSteps; i++) {
          sprintf(timeoutName, "autoVol%i", i);
          cancel_timeout(timeoutName);
      }
      
      if (smoothedVolume != nullptr) {
        int locDelay = 0;
        float locVolume = *smoothedVolume;
        float volStep = (static_cast<float>(targetVolume) - *smoothedVolume) / (volAnimationSteps + 1);
        for (int i = 0; i < volAnimationSteps; i++) {
          sprintf(timeoutName, "autoVol%i", i);
          this->set_timeout(timeoutName, locDelay, [this, smoothedVolume, volStep, targetVolume, i, page]() {
            if (i == volAnimationSteps - 1) {
              *smoothedVolume = static_cast<float>(targetVolume);
            } else {
              *smoothedVolume = *smoothedVolume += volStep;
            }
            this->oledScreen->show_page(page);
            this->oledScreen->update();
          });
          locDelay += volAnimationTime / volAnimationSteps;
        }
      } else {
        this->oledScreen->show_page(page);
        this->oledScreen->update();
      }

      this->set_timeout("autoOff", this->delay, [this]() {
        this->oledScreen->show_page(this->offPage);
        this->oledScreen->update();
      });
    }
};

LogitechZ906ScreenTimeout* screen_timeout;
```
