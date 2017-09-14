
/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * struct sensors_poll_device_1 is used in HAL versions >= SENSORS_DEVICE_API_VERSION_1_0
 */
typedef struct samsung_sensors_poll_device_1 {
    union {
        /* sensors_poll_device_1 is compatible with sensors_poll_device_t,
         * and can be down-cast to it
         */
        struct sensors_poll_device_t v0;

        struct {
            struct hw_device_t common;

            /* Activate/de-activate one sensor. Return 0 on success, negative
             *
             * sensor_handle is the handle of the sensor to change.
             * enabled set to 1 to enable, or 0 to disable the sensor.
             *
             * Return 0 on success, negative errno code otherwise.
             */
            int (*activate)(struct sensors_poll_device_t *dev,
                    int sensor_handle, int enabled);

            /**
             * Set the events's period in nanoseconds for a given sensor.
             * If sampling_period_ns > max_delay it will be truncated to
             * max_delay and if sampling_period_ns < min_delay it will be
             * replaced by min_delay.
             */
            int (*setDelay)(struct sensors_poll_device_t *dev,
                    int sensor_handle, int64_t sampling_period_ns);

            /**
             * Returns an array of sensor data.
             */
            int (*poll)(struct sensors_poll_device_t *dev,
                    sensors_event_t* data, int count);
        };
    };


    /*
     * Sets a sensor’s parameters, including sampling frequency and maximum
     * report latency. This function can be called while the sensor is
     * activated, in which case it must not cause any sensor measurements to
     * be lost: transitioning from one sampling rate to the other cannot cause
     * lost events, nor can transitioning from a high maximum report latency to
     * a low maximum report latency.
     * See the Batching sensor results page for details:
     * http://source.android.com/devices/sensors/batching.html
     */
    int (*batch)(struct sensors_poll_device_1* dev,
            int sensor_handle, int flags, int64_t sampling_period_ns,
            int64_t max_report_latency_ns);

    /*
     * Flush adds a META_DATA_FLUSH_COMPLETE event (sensors_event_meta_data_t)
     * to the end of the "batch mode" FIFO for the specified sensor and flushes
     * the FIFO.
     * If the FIFO is empty or if the sensor doesn't support batching (FIFO size zero),
     * it should return SUCCESS along with a trivial META_DATA_FLUSH_COMPLETE event added to the
     * event stream. This applies to all sensors other than one-shot sensors.
     * If the sensor is a one-shot sensor, flush must return -EINVAL and not generate
     * any flush complete metadata.
     * If the sensor is not active at the time flush() is called, flush() should return
     * -EINVAL.
     */
    int (*flush)(struct sensors_poll_device_1* dev, int sensor_handle);

    void (*reserved_procs[8])(void);

} samsung_sensors_poll_device_1_t;