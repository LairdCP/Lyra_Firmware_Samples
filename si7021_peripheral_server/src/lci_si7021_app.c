/**
 * @file lci_si7021_app.c
 * @brief SI7021 and GATT application interface
 *
 * Copyright (c) 2020-2021 Laird Connectivity
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *  Created on: Aug 26, 2021
 *      Author: Alexander.Brezinov
 */
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_simple_timer.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_button_instances.h"
#include "sl_gatt_service_rht.h"
#include "sl_sensor_rht.h"
/* Simple timer timeout in milliseconds */
#define ADV_TIMER_TIMEOUT_MS  1000
/* LED instance selection*/
#define ADV_IND_LED           SL_SIMPLE_LED_INSTANCE(0)
/* The advertising set handle allocated from Bluetooth stack */
static uint8_t advertising_set_handle = 0xff;
/* ASCII code for degree celsious sign */
static const uint8_t celsious_ascii_code = 248;
/* Simple timer for controlling an LED#0 during advertising */
static sl_simple_timer_t adv_timer;
/* Simple timer local functions */
static void hdl_adv_timer_event(sl_simple_timer_t *timer, void *data);
static void adv_start_timer(void);
static void adv_stop_timer(void);
/**
* @brief Simple timer handler
 *
* @param[in] timer resource pointer
* @param[in] data pointer
*
* @retval None
*/
static void hdl_adv_timer_event(sl_simple_timer_t *timer, void *data) {
  (void)timer;
  (void)data;
  sl_led_toggle(ADV_IND_LED);
}
/**
* @brief Simple timer start procedure
 *
* @param[in] None
*
* @retval None
*/
static void adv_start_timer(void)
{
  sl_status_t sc;
  sc = sl_simple_timer_start(
         &adv_timer,
         ADV_TIMER_TIMEOUT_MS,
         hdl_adv_timer_event,
         NULL,
         true);
  app_assert_status(sc);
}
/**
* @brief Simple timer stop procedure
 *
* @param[in] None
*
* @retval None
*/
static void adv_stop_timer(void)
{
  sl_status_t sc;
  sc = sl_simple_timer_stop(&adv_timer);
  app_assert_status(sc);
  sl_led_turn_on(ADV_IND_LED);
}
/**
* @brief Application initialization procedure
 *
* @param[in] None
*
* @retval None
*/
void app_init(void)
{
  app_log_info("[SI7021 sensor] Laird Connectivity simple peripheral server demo");
  app_log_nl();
}
/**
* @brief Bluetooth events handler
 *
* @param[in] evt Bluetooth message pointer
*
* @retval None
*/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];

  switch (SL_BT_MSG_ID(evt->header)) {
    /* ------------------------------- */
    /* This event indicates the device has started and the radio is ready
     * Do not call any stack command before receiving this boot event!
     *
     */
    case sl_bt_evt_system_boot_id:

      /* Extract unique ID from BT Address */
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);

      /* Pad and reverse unique ID to get System ID */
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      app_assert_status(sc);

      /* Create an advertising set */
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      /* Set advertising interval to 100ms */
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, /* min. adv. interval (milliseconds * 1.6) */
        160, /* max. adv. interval (milliseconds * 1.6) */
        0,   /* adv. duration */
        0);  /* max. num. adv. events */
      app_assert_status(sc);
      /* Start general advertising and enable connections */
      sc = sl_bt_advertiser_start(
        advertising_set_handle,
        sl_bt_advertiser_general_discoverable,
        sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      adv_start_timer();
      break;

    /* ------------------------------- */
    /* This event indicates that a new connection was opened */
    case sl_bt_evt_connection_opened_id:
      adv_stop_timer();
      break;

    /* ------------------------------- */
    /* This event indicates that a connection was closed */
    case sl_bt_evt_connection_closed_id:
      /* Restart advertising after client has disconnected */
      sc = sl_bt_advertiser_start(
        advertising_set_handle,
        sl_bt_advertiser_general_discoverable,
        sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      adv_start_timer();
      break;

    /* ------------------------------- */
    /* Default event handler */
    default:
      break;
  }
}
/**
* @brief Button handler
 *
* @param[in] handle Pointer to button resource
*
* @retval None
*/
void sl_button_on_change(const sl_button_t *handle)
{
  (void)handle;
  if(sl_button_get_state(handle)) {
      app_log_info("BTN#0 is pushed \n");
  } else {
      app_log_info("BTN#0 is released \n");
  }
}
/**
* @brief Humidity and Temperature GATT handler
 *
* @param[in] rh relative humidity value
* @param[in] t  temperature value
*
* @retval sl_status SL_STATUS_OK, if the reading from the sensor was successful,
*                   value greater than zero indicates error in sensor reading
*/
sl_status_t sl_gatt_service_rht_get(uint32_t *rh, int32_t *t)
{
  sl_status_t sc;
  sc = sl_sensor_rht_get(rh, t);
  if (SL_STATUS_OK == sc) {
    app_log_info("Humidity [relative humidity as a percentage] - %3.2f %%RH", (float)*rh / 1000.0f);
    app_log_nl();
    app_log_info("Temperature [degree celsius] - %3.2f %cC", (float)*t / 1000.0f, celsious_ascii_code);
    app_log_nl();
  } else {
    app_log_status_error_f(sc, "RHT sensor measurement failed");
    app_log_nl();
  }
  return sc;
}
