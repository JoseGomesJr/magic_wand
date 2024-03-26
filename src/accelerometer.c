
#include "accelerometer.h"

#define ACCELEROMETER_NODE DT_ALIAS(accel0)

#define BUFLEN 150
int begin_index = 0;
const struct device *const sensor = DEVICE_DT_GET(ACCELEROMETER_NODE);
int current_index = 0;

float bufx[BUFLEN] = { 0.0f };
float bufy[BUFLEN] = { 0.0f };
float bufz[BUFLEN] = { 0.0f };

bool initial = true;


#define ALPHA 0.9f
static float leitura_anterior[3] = {0.0};

float filtroPassaAlta(float leitura_atual, int eixo) {
    return ALPHA * leitura_anterior[eixo] + (1 - ALPHA) * leitura_atual;
}

int accelerometer_config()
{
    if (!device_is_ready(sensor)) {
        printk("%s: device not ready.\n", sensor->name);
        return -EACCES;
    }

    if (sensor == NULL) {
        printk("Failed to get accelerometer, name: %s\n",
                    sensor->name);
    } else {
        printk("Got accelerometer, name: %s\n",
                    sensor->name);
    }
    return 0;
}

bool accelerometer_read(float *input, int length)
{
    int rc;
    struct sensor_value accel[3];
    double acc_x, acc_y, acc_z;
    int samples_count;

    rc = sensor_sample_fetch(sensor);
    if (rc < 0) {
        printk("Fetch failed\n");
        return false;
    }
    /* Skip if there is no data */
    if (rc) {
        return false;
    }

    rc = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, accel);
    if (rc < 0) {
        printk("ERROR: Update failed: %d\n", rc);
        return false;
    }

    acc_x = sensor_value_to_double(&accel[0]);
    acc_y = sensor_value_to_double(&accel[1]);
    acc_z = sensor_value_to_double(&accel[2]);

    acc_x = (double) filtroPassaAlta((float )acc_x, 0);
    acc_y = (double) filtroPassaAlta((float )acc_y, 1);
    acc_z = (double) filtroPassaAlta((float )acc_z, 2);


    input[0] = (float ) acc_x;
    input[1] = (float ) acc_y;
    input[2] = (float ) acc_z;

    return true;
}