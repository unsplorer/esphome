import esphome.codegen as cg
from esphome.components.display import (
    BASIC_DISPLAY_SCHEMA,
    DisplayBuffer,
    register_display,
)
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_ROTATION

AUTO_LOAD = ["display"]
DEPENDENCIES = ["spi"]

mpsmv2_tft_ns = cg.esphome_ns.namespace("mpsmv2_tft")
MPSMV2_TFT = mpsmv2_tft_ns.class_("MPSMV2_TFT", cg.PollingComponent, DisplayBuffer)

CONFIG_SCHEMA = BASIC_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(MPSMV2_TFT),
        cv.Optional(CONF_ROTATION, default=0): cv.int_,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA],
            [(MPSMV2_TFT.operator("ref"), "it")],
            return_type=cg.void,
        )
        cg.add(var.set_writer(lambda_))

    cg.add(var.set_rotation(config[CONF_ROTATION]))
    await register_display(var, config)
