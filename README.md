# Radiance Soil Monitoring

This is a monorepo for a self hosted soil moisture monitoring stack. The monitoring
is done with a Particle device collecting data from a capacitive soil moisture sensor
and sent to an MQTT broker.

## Particle Firmware

The firmware for the particle board is responsible for collecting the data
from the sensor and send it to the MQTT broker (see below). Currently you
will need to edit the host manually yourself before flashing your device.

The firmware has QOS1 for messages and also handles managing disconnections automatically.

### Firmware TODO

- Batch up failed payloads so they can be retried later.

## MQTT Broker and Database

Processing and storing messages is powered by [Machbase Neo](https://docs.machbase.com/neo/).

### Initializing Database

To start you will have to create the table that stores our sensor data. Database
queries can be made by connecting to the MQTT broker and sending queries shaped like
the below to the `db/query` topic.

```plaintext
{
    "q": "create tag table if not exists moisture (id varchar(50) primary key, 
           time datetime basetime, value double summarized, device_id varchar(20));",
    "format": "csv",
    "reply": "db/reply/my_query"
}
# to verify that the table is there
{
    "q": "SELECT * FROM moisture;",
    "format": "csv",
    "reply": "db/reply/my_query"
}
```

### Writing Data

For now writing data is typically handled by the particle firmware. However for
informational purposes, you write data by publishing messages to `db/append/table_name`.
If using the schema above, will be `db/append/moisture`. with the content of:

```plaintext
[["id_value", 1, 1.04, "device_id_1"], ["id_value2", 2, 9.44, "device_id_1"]]
<!-- or -->
["id_value2", 2, 9.44, "device_id_1"]
```

### MQTT TODO

- Automate schema init
