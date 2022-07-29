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
/* Bluetooth Low Energy connection parameters */
#define CONN_INTERVAL_MIN             80   /* 100 milliseconds */
#define CONN_INTERVAL_MAX             80   /* 100 milliseconds */
#define CONN_RESPONDER_LATENCY        0    /* no latency */
#define CONN_TIMEOUT                  100  /* 1000 milliseconds */
#define CONN_MIN_CE_LENGTH            0
#define CONN_MAX_CE_LENGTH            0xffff
/* Bluetooth Low Energy scanning parameters */
#define SCAN_INTERVAL                 16   /* 10 milliseconds */
#define SCAN_WINDOW                   16   /* 10 milliseconds */
#define SCAN_PASSIVE                  0
/* Temperature and humidity invalidated values */
#define TEMP_INVALID                  0
#define HUM_INVALID                   0
/* Bluetooth Low Energy invalidated  handles */
#define CONNECTION_HANDLE_INVALID     ((uint8_t)0xFFu)
#define SERVICE_HANDLE_INVALID        ((uint32_t)0xFFFFFFFFu)
#define CHARACTERISTIC_HANDLE_INVALID ((uint16_t)0xFFFFu)
#define TABLE_INDEX_INVALID           ((uint8_t)0xFFu)
/* Minimum number of connections is one */
#if SL_BT_CONFIG_MAX_CONNECTIONS < 1
  #error At least 1 connection has to be enabled!
#endif
/* Connection's states */
typedef enum {
  scanning,
  opening,
  discover_services,
  discover_characteristics,
  enable_indication,
  running
} conn_state_t;
/* Connection's property structure */
typedef struct {
  uint8_t  connection_handle;
  uint16_t server_address;
  uint32_t envsens_service_handle;
  uint16_t envsens_humidity_characteristic_handle;
  uint16_t envsens_temp_characteristic_handle;
  int16_t temp;
  uint16_t humidity;
} conn_properties_t;
/* Array for holding properties of multiple (parallel) connections */
static conn_properties_t conn_properties[SL_BT_CONFIG_MAX_CONNECTIONS];
/* Counter of active connections */
static uint8_t active_connections_num;
/* State of the connection under establishment */
static conn_state_t conn_state;
/* Environmental Sensing service UUID defined by Bluetooth SIG */
static const uint8_t envsens_service[2] = { 0x1A, 0x18 };
/* Environmental Sensing Humidity characteristic UUID defined by Bluetooth SIG */
static const uint8_t envsens_humidity_char[2] = { 0x6f, 0x2a };
/* Environmental Sensing Temperature characteristic UUID defined by Bluetooth SIG */
static const uint8_t envsens_temp_char[2] = { 0x6e, 0x2a };
/* GATT read temperature flag */
static bool bf_read_temp;
/* ASCII code for degree celsious sign */
static const uint8_t celsious_ascii_code = 248;
/* Local functions for handling BLuetooth Low Energy scanning and connections */
static void init_properties(void);
static uint8_t find_service_in_advertisement(uint8_t *data, uint8_t len);
static uint8_t find_index_by_connection_handle(uint8_t connection);
static void add_connection(uint8_t connection, uint16_t address);
static void remove_connection(uint8_t connection);
static bd_addr *read_and_cache_bluetooth_address(uint8_t *address_type_out);
static void print_bluetooth_address(void);
/**
* @brief Initialize connection properties
 *
* @param[in] None
*
* @retval None
*/
static void init_properties(void)
{
  uint8_t i;
  active_connections_num = 0;

  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    conn_properties[i].connection_handle = CONNECTION_HANDLE_INVALID;
    conn_properties[i].envsens_service_handle = SERVICE_HANDLE_INVALID;
    conn_properties[i].envsens_humidity_characteristic_handle = CHARACTERISTIC_HANDLE_INVALID;
    conn_properties[i].envsens_temp_characteristic_handle = CHARACTERISTIC_HANDLE_INVALID;
    conn_properties[i].humidity = HUM_INVALID;
    conn_properties[i].temp = TEMP_INVALID;
  }
}
/**
* @brief Parse advertisements looking for environment sensing service
 *
* @param[in] data pointer to advertising data
* @param[in] len  length of advertising data
*
* @retval 1 if Environmental sensing service UUID is found in advertising,
*         0 otherwise
*/
static uint8_t find_service_in_advertisement(uint8_t *data, uint8_t len)
{
  uint8_t ad_field_length;
  uint8_t ad_field_type;
  uint8_t i = 0;
  /* Parse advertisement packet */
  while (i < len) {
    ad_field_length = data[i];
    ad_field_type = data[i + 1];
    if (ad_field_type == 0x02 || ad_field_type == 0x03) {
      /* Compare UUID to Environmental sensing service UUID */
      if (memcmp(&data[i + 2], envsens_service, 2) == 0) {
        return 1;
      }
    }
    /* Advance to the next AD structure */
    i = i + ad_field_length + 1;
  }
  return 0;
}
/**
* @brief Find the index of a given connection in the connection_properties array
 *
* @param[in] connection connection's handle
*
* @retval valid connection index if the handle is in the list
*         invalid index otherwise
*/
static uint8_t find_index_by_connection_handle(uint8_t connection)
{
  for (uint8_t i = 0; i < active_connections_num; i++) {
    if (conn_properties[i].connection_handle == connection) {
      return i;
    }
  }
  return TABLE_INDEX_INVALID;
}
/**
* @brief Add a new connection to the connection_properties array
 *
* @param[in] connection connection's handle
* @param[in] address server address
*
* @retval None
*
*/
static void add_connection(uint8_t connection, uint16_t address)
{
  conn_properties[active_connections_num].connection_handle = connection;
  conn_properties[active_connections_num].server_address    = address;
  active_connections_num++;
}
/**
* @brief Remove a connection from the connection_properties array
 *
* @param[in] connection connection's handle
*
* @retval None
*
*/
static void remove_connection(uint8_t connection)
{
  uint8_t i;
  uint8_t table_index = find_index_by_connection_handle(connection);

  if (active_connections_num > 0) {
    active_connections_num--;
  }
  for (i = table_index; i < active_connections_num; i++) {
    conn_properties[i] = conn_properties[i + 1];
  }
  for (i = active_connections_num; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    conn_properties[i].connection_handle = CONNECTION_HANDLE_INVALID;
    conn_properties[i].envsens_service_handle = SERVICE_HANDLE_INVALID;
    conn_properties[i].envsens_humidity_characteristic_handle = CHARACTERISTIC_HANDLE_INVALID;
    conn_properties[i].humidity = HUM_INVALID;
    conn_properties[i].envsens_temp_characteristic_handle = CHARACTERISTIC_HANDLE_INVALID;
    conn_properties[i].temp = TEMP_INVALID;
  }
}
/**
* @brief Function to Read and Cache Bluetooth Address
 *
* @param[in] address_type_out A pointer to the outgoing address_type
*
* @retval Pointer to the cached Bluetooth Address
*
*/
static bd_addr *read_and_cache_bluetooth_address(uint8_t *address_type_out)
{
  static bd_addr address;
  static uint8_t address_type;
  static bool cached = false;

  if (!cached) {
    sl_status_t sc = sl_bt_system_get_identity_address(&address, &address_type);
    app_assert_status(sc);
    cached = true;
  }

  if (address_type_out) {
    *address_type_out = address_type;
  }

  return &address;
}
/**
* @brief Function to Print Bluetooth Address
 *
* @param[in] None
*
* @retval None
*
*/
static void print_bluetooth_address(void)
{
  uint8_t address_type;
  bd_addr *address = read_and_cache_bluetooth_address(&address_type);

  app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
               address_type ? "static random" : "public device",
               address->addr[5],
               address->addr[4],
               address->addr[3],
               address->addr[2],
               address->addr[1],
               address->addr[0]);
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
  bf_read_temp = false;
  /* Initialize connection properties */
  init_properties();
  app_log_info("[SI7021 sensor] Laird Connectivity simple central client demo\n");
}
/**
* @brief Bluetooth events handler
 *
* @param[in] evt Bluetooth message pointer
*
* @retval None
*/
void sl_bt_on_event(sl_bt_msg_t* evt)
{
  sl_status_t sc;
  uint8_t *char_value;
  uint8_t char_value_len;
  uint16_t addr_value;
  uint8_t table_index;
  /* Handle stack events */
  switch (SL_BT_MSG_ID(evt->header)) {
    /* ------------------------------- */
    /* This event indicates the device has started and the radio is ready */
    /* Do not call any stack command before receiving this boot event! */
    case sl_bt_evt_system_boot_id:
      /* Print boot message */
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d\n",
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);
      /* Print bluetooth address */
      print_bluetooth_address();
      /* Set passive scanning on 1Mb PHY */
      sc = sl_bt_scanner_set_mode(sl_bt_gap_1m_phy, SCAN_PASSIVE);
      app_assert_status(sc);
      /* Set scan interval and scan window */
      sc = sl_bt_scanner_set_timing(sl_bt_gap_1m_phy, SCAN_INTERVAL, SCAN_WINDOW);
      app_assert_status(sc);
      /* Set the default connection parameters for subsequent connections */
      sc = sl_bt_connection_set_default_parameters(CONN_INTERVAL_MIN,
                                                   CONN_INTERVAL_MAX,
                                                   CONN_RESPONDER_LATENCY,
                                                   CONN_TIMEOUT,
                                                   CONN_MIN_CE_LENGTH,
                                                   CONN_MAX_CE_LENGTH);
      app_assert_status(sc);
      /* Start scanning - looking for environmental sensing devices */
      sc = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
      app_assert_status_f(sc,
                          "Failed to start discovery #1\n");
      conn_state = scanning;
      break;
    /* ------------------------------- */
    /* This event is generated when an advertisement packet or a scan response */
    /* is received from a responder */
    case sl_bt_evt_scanner_scan_report_id:
      /* Parse advertisement packets */
      if (evt->data.evt_scanner_scan_report.packet_type == 0) {
        /* If an environmental sensing advertisement is found... */
        if (find_service_in_advertisement(&(evt->data.evt_scanner_scan_report.data.data[0]),
                                          evt->data.evt_scanner_scan_report.data.len) != 0) {
          /* then stop scanning for a while */
          sc = sl_bt_scanner_stop();
          app_assert_status(sc);
          /* and connect to that device */
          if (active_connections_num < SL_BT_CONFIG_MAX_CONNECTIONS) {
            sc = sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                       evt->data.evt_scanner_scan_report.address_type,
                                       sl_bt_gap_1m_phy,
                                       NULL);
            app_assert_status(sc);
            conn_state = opening;
          }
        }
      }
      break;
    /* ------------------------------- */
    /* This event is generated when a new connection is established */
    case sl_bt_evt_connection_opened_id:
      /* Get last two bytes of sender address */
      addr_value = (uint16_t)(evt->data.evt_connection_opened.address.addr[1] << 8) + evt->data.evt_connection_opened.address.addr[0];
      /* Add connection to the connection_properties array */
      add_connection(evt->data.evt_connection_opened.connection, addr_value);
      /* Discover environment sensing service on the responder device */
      sc = sl_bt_gatt_discover_primary_services_by_uuid(evt->data.evt_connection_opened.connection,
                                                        sizeof(envsens_service),
                                                        (const uint8_t*)envsens_service);
      app_assert_status(sc);
      /* Set remote connection power reporting - needed for Power Control */
      sc = sl_bt_connection_set_remote_power_reporting(
        evt->data.evt_connection_opened.connection,
        sl_bt_connection_power_reporting_enable);
      app_assert_status(sc);
      conn_state = discover_services;
      break;
    /* ------------------------------- */
    /* This event is generated when a new service is discovered */
    case sl_bt_evt_gatt_service_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_service.connection);
      if (table_index != TABLE_INDEX_INVALID) {
        /* Save service handle for future reference */
        conn_properties[table_index].envsens_service_handle = evt->data.evt_gatt_service.service;
      }
      break;
    /* ------------------------------- */
    /* This event is generated when a new characteristic is discovered */
    case sl_bt_evt_gatt_characteristic_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_characteristic.connection);
      if (table_index != TABLE_INDEX_INVALID) {
        /* Save characteristic handle for future reference */
        if(evt->data.evt_gatt_characteristic.uuid.data[0] == envsens_humidity_char[0]) {
          conn_properties[table_index].envsens_humidity_characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
        }
        if(evt->data.evt_gatt_characteristic.uuid.data[0] == envsens_temp_char[0]) {
          conn_properties[table_index].envsens_temp_characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
        }
      }
      break;
    /* ------------------------------- */
    /* This event is generated for various procedure completions, e.g. when a */
    /* write procedure is completed, or service discovery is completed */
    case sl_bt_evt_gatt_procedure_completed_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_procedure_completed.connection);
      if (table_index == TABLE_INDEX_INVALID) {
        break;
      }
      /* If service discovery finished */
      if (conn_state == discover_services && conn_properties[table_index].envsens_service_handle != SERVICE_HANDLE_INVALID) {
        sc = sl_bt_gatt_discover_characteristics(evt->data.evt_gatt_procedure_completed.connection,
                                                 conn_properties[table_index].envsens_service_handle);
        app_assert_status(sc);
        conn_state = discover_characteristics;
        break;
      }
      /* If characteristic discovery finished */
      if (conn_state == discover_characteristics && conn_properties[table_index].envsens_humidity_characteristic_handle != CHARACTERISTIC_HANDLE_INVALID && bf_read_temp == false) {
        sl_bt_scanner_stop();
        sc = sl_bt_gatt_read_characteristic_value(evt->data.evt_gatt_procedure_completed.connection,
                                                  conn_properties[table_index].envsens_humidity_characteristic_handle);
        app_assert_status(sc);
        bf_read_temp = true;
        break;
      }
      if (conn_state == discover_characteristics && conn_properties[table_index].envsens_temp_characteristic_handle != CHARACTERISTIC_HANDLE_INVALID && bf_read_temp == true) {
        sl_bt_scanner_stop();
        sc = sl_bt_gatt_read_characteristic_value(evt->data.evt_gatt_procedure_completed.connection,
                                             conn_properties[table_index].envsens_temp_characteristic_handle);
        app_assert_status(sc);
        bf_read_temp = false;
        break;
      }
      break;
    /* ------------------------------- */
    /* This event is generated when GATT characteristic is read */
    case sl_bt_evt_gatt_characteristic_value_id:
      char_value_len = evt->data.evt_gatt_characteristic_value.value.len;
      if(char_value_len >= sizeof(uint16_t)) {
        char_value = &(evt->data.evt_gatt_characteristic_value.value.data[0]);
        table_index = find_index_by_connection_handle(evt->data.evt_gatt_characteristic_value.connection);
        if (table_index != TABLE_INDEX_INVALID) {
            if(evt->data.evt_gatt_characteristic_value.characteristic == conn_properties[table_index].envsens_temp_characteristic_handle) {
                memcpy(&conn_properties[table_index].temp, &char_value[0], char_value_len);
                app_log_info("Temperature [degree celsius] - %3.2f %cC", (float)conn_properties[table_index].temp / 100.0f, celsious_ascii_code);
                app_log_nl();
            }
            if(evt->data.evt_gatt_characteristic_value.characteristic == conn_properties[table_index].envsens_humidity_characteristic_handle) {
                memcpy(&conn_properties[table_index].humidity, &char_value[0], char_value_len);
                app_log_info("Humidity [relative humidity as a percentage] - %3.2f %%RH", (float)conn_properties[table_index].humidity / 100.0f);
                app_log_nl();
            }
        }
      } else {
          app_log_warning("Characteristic value too short: %d\n",
                          evt->data.evt_gatt_characteristic_value.value.len);
      }
      break;
    /* ------------------------------- */
    /* This event is generated when a connection is dropped */
    case sl_bt_evt_connection_closed_id:
      /* remove connection from active connections */
      remove_connection(evt->data.evt_connection_closed.connection);
      if (conn_state != scanning) {
        /* start scanning again to find new devices */
        sc = sl_bt_scanner_start(sl_bt_gap_1m_phy, sl_bt_scanner_discover_generic);
        app_assert_status_f(sc,
                            "Failed to start discovery #3\n");
        conn_state = scanning;
      }
      break;
    default:
      break;
  }
}
