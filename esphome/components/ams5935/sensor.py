import esphome.codegen as cg
from esphome.components import i2c, sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_MODEL,
    CONF_OVERSAMPLING,
    CONF_PRESSURE,
    CONF_TEMPERATURE,
    CONF_UPDATE_INTERVAL,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PASCAL,
)

DEPENDENCIES = ["i2c"]
# AUTO_LOAD = ["sensor"]

ams5935_ns = cg.esphome_ns.namespace("ams5935")
TRANSDUCER = ams5935_ns.enum("Ams5935::Transducer")
TRANSDUCER_TYPE = {
    "AMS5935-0002-D": TRANSDUCER.AMS5935_0002_D,
    "AMS5935-0005-D": TRANSDUCER.AMS5935_0005_D,
    "AMS5935-0010-D": TRANSDUCER.AMS5935_0010_D,
    "AMS5935-0020-D": TRANSDUCER.AMS5935_0020_D,
    "AMS5935-0035-D": TRANSDUCER.AMS5935_0035_D,
    "AMS5935-0050-D": TRANSDUCER.AMS5935_0050_D,
    "AMS5935-0100-D": TRANSDUCER.AMS5935_0100_D,
    "AMS5935-0200-D": TRANSDUCER.AMS5935_0200_D,
    "AMS5935-0350-D": TRANSDUCER.AMS5935_0350_D,
    "AMS5935-0500-D": TRANSDUCER.AMS5935_0500_D,
    "AMS5935-1000-D": TRANSDUCER.AMS5935_1000_D,
    "AMS5935-0002-D-N": TRANSDUCER.AMS5935_0002_D_N,
    "AMS5935-0005-D-N": TRANSDUCER.AMS5935_0005_D_N,
    "AMS5935-0010-D-N": TRANSDUCER.AMS5935_0010_D_N,
    "AMS5935-0020-D-N": TRANSDUCER.AMS5935_0020_D_N,
    "AMS5935-0100-D-N": TRANSDUCER.AMS5935_0100_D_N,
    "AMS5935-0050-D-N": TRANSDUCER.AMS5935_0050_D_N,
    "AMS5935-0200-D-N": TRANSDUCER.AMS5935_0200_D_N,
    "AMS5935-0350-D-N": TRANSDUCER.AMS5935_0350_D_N,
    "AMS5935-1000-D-N": TRANSDUCER.AMS5935_1000_D_N,
    "AMS5935-0001-D-B": TRANSDUCER.AMS5935_0001_D_B,
    "AMS5935-0002-D-B": TRANSDUCER.AMS5935_0002_D_B,
    "AMS5935-0005-D-B": TRANSDUCER.AMS5935_0005_D_B,
    "AMS5935-0010-D-B": TRANSDUCER.AMS5935_0010_D_B,
    "AMS5935-0020-D-B": TRANSDUCER.AMS5935_0020_D_B,
    "AMS5935-0035-D-B": TRANSDUCER.AMS5935_0035_D_B,
    "AMS5935-0050-D-B": TRANSDUCER.AMS5935_0050_D_B,
    "AMS5935-0100-D-B": TRANSDUCER.AMS5935_0100_D_B,
    "AMS5935-0200-D-B": TRANSDUCER.AMS5935_0200_D_B,
    "AMS5935-0350-D-B": TRANSDUCER.AMS5935_0350_D_B,
    "AMS5935-0500-D-B": TRANSDUCER.AMS5935_0500_D_B,
    "AMS5935-1000-D-B": TRANSDUCER.AMS5935_1000_D_B,
    "AMS5935-0001-D-B-N": TRANSDUCER.AMS5935_0001_D_B_N,
    "AMS5935-0002-D-B-N": TRANSDUCER.AMS5935_0002_D_B_N,
    "AMS5935-0005-D-B-N": TRANSDUCER.AMS5935_0005_D_B_N,
    "AMS5935-0010-D-B-N": TRANSDUCER.AMS5935_0010_D_B_N,
    "AMS5935-0020-D-B-N": TRANSDUCER.AMS5935_0020_D_B_N,
    "AMS5935-0050-D-B-N": TRANSDUCER.AMS5935_0050_D_B_N,
    "AMS5935-0100-D-B-N": TRANSDUCER.AMS5935_0100_D_B_N,
    "AMS5935-0200-D-B-N": TRANSDUCER.AMS5935_0200_D_B_N,
    "AMS5935-0350-D-B-N": TRANSDUCER.AMS5935_0350_D_B_N,
    "AMS5935-1000-D-B-N": TRANSDUCER.AMS5935_1000_D_B_N,
    "AMS5935-0500-A": TRANSDUCER.AMS5935_0500_A,
    "AMS5935-1000-A": TRANSDUCER.AMS5935_1000_A,
    "AMS5935-1500-A": TRANSDUCER.AMS5935_1500_A,
    "AMS5935-2000-A": TRANSDUCER.AMS5935_2000_A,
    "AMS5935-1200-B": TRANSDUCER.AMS5935_1200_B,
}

Ams5935 = ams5935_ns.class_("Ams5935", cg.PollingComponent, i2c.I2CDevice)
# CONF_OVERSAMPLING = "oversampling"
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Ams5935),
            cv.Required(CONF_MODEL): cv.enum(TRANSDUCER_TYPE, upper=True),
            cv.Optional(CONF_OVERSAMPLING, default=False): cv.boolean,
            cv.Optional(CONF_PRESSURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_PASCAL,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_PRESSURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema(CONF_UPDATE_INTERVAL))
    .extend(i2c.i2c_device_schema(0x28))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_transducer_type(config.get(CONF_MODEL)))
    cg.add(var.set_oversampling(config.get(CONF_OVERSAMPLING)))

    if pressure_config := config.get(CONF_PRESSURE):
        sens = await sensor.new_sensor(pressure_config)
        cg.add(var.set_pressure_sensor(sens))

    if temperature_config := config.get(CONF_TEMPERATURE):
        sens = await sensor.new_sensor(temperature_config)
        cg.add(var.set_temperature_sensor(sens))
