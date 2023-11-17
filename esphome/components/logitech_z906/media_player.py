import esphome.codegen as cg
from esphome import automation
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_UART_ID,
    CONF_TRIGGER_ID
)
from esphome.components import uart, media_player

DEPENDENCIES = ['uart']
AUTO_LOAD = ['media_player', 'uart']
CODEOWNERS = ["@paco3346"]

CONF_ON_VOLUME_CHANGE_REAR = "on_volume_change_rear"
CONF_ON_VOLUME_CHANGE_CENTER = "on_volume_change_center"
CONF_ON_VOLUME_CHANGE_SUB = "on_volume_change_sub"

logitech_z906_ns = cg.esphome_ns.namespace("logitech_z906")
LogitechZ906Component = logitech_z906_ns.class_("LogitechZ906MediaPlayer", cg.Component, media_player.MediaPlayer)

VolumeChangeRearTrigger = logitech_z906_ns.class_("VolumeChangeRearTrigger", automation.Trigger.template())
VolumeChangeCenterTrigger = logitech_z906_ns.class_("VolumeChangeCenterTrigger", automation.Trigger.template())
VolumeChangeSubTrigger = logitech_z906_ns.class_("VolumeChangeSubTrigger", automation.Trigger.template())

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LogitechZ906Component),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(media_player.MEDIA_PLAYER_SCHEMA)
    .extend({
        cv.Optional(CONF_ON_VOLUME_CHANGE_REAR): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(VolumeChangeRearTrigger),
            }
        ),
        cv.Optional(CONF_ON_VOLUME_CHANGE_CENTER): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(VolumeChangeCenterTrigger),
            }
        ),
        cv.Optional(CONF_ON_VOLUME_CHANGE_SUB): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(VolumeChangeSubTrigger),
            }
        ),
    })
)


async def setup_triggers_(var, config):
    for conf in config.get(CONF_ON_VOLUME_CHANGE_REAR, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)
    for conf in config.get(CONF_ON_VOLUME_CHANGE_CENTER, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)
    for conf in config.get(CONF_ON_VOLUME_CHANGE_SUB, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)


async def to_code(config):
    uart_component = await cg.get_variable(config[CONF_UART_ID])
    var = cg.new_Pvariable(config[CONF_ID], uart_component)
    cg.add(var.set_uart_name(str(config[CONF_UART_ID])))
    await cg.register_component(var, config)
    await media_player.register_media_player(var, config)
    cg.add(var.set_up_surround_speakers())
    await setup_triggers_(var, config)
