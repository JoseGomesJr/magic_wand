
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

    samples_count = 100;
    for (int i = 0; i < samples_count; i++) {
        rc = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, accel);
        if (rc < 0) {
            printk("ERROR: Update failed: %d\n", rc);
            return false;
        }
        bufx[begin_index] = (float)sensor_value_to_double(&accel[0]);
        bufy[begin_index] = (float)sensor_value_to_double(&accel[1]);
        bufz[begin_index] = (float)sensor_value_to_double(&accel[2]);

        bufx[begin_index] = filtroPassaAlta(bufx[begin_index], 0);
        bufy[begin_index] = filtroPassaAlta(bufy[begin_index], 1);
        bufz[begin_index]= filtroPassaAlta(bufy[begin_index], 2);

        begin_index++;
        if (begin_index >= BUFLEN) {
            begin_index = 0;
        }
    }

    if (initial && begin_index >= 100) {
        initial = false;
    }

    if (initial) {
        return false;
    }

    int sample = 0;
    for (int i = 0; i < (length - 3); i += 3) {
        int ring_index = begin_index + sample - length / 3;
        if (ring_index < 0) {
            ring_index += BUFLEN;
        }
        input[i] = bufx[ring_index];
        input[i + 1] = bufy[ring_index];
        input[i + 2] = bufz[ring_index];
        sample++;
    }

    return true;
}