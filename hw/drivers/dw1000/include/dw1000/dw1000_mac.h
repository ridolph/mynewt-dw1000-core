/**
 * Copyright 2018, Decawave Limited, All Rights Reserved 
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _DW1000_MAC_H_
#define _DW1000_MAC_H_

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <hal/hal_spi.h>
#include <dw1000/dw1000_regs.h>
#include <dw1000/dw1000_dev.h>

#define DWT_DEVICE_ID   (0xDECA0130)        //!< DW1000 MP device ID

//! constants for selecting the bit rate for data TX (and RX)
//! These are defined for write (with just a shift) the TX_FCTRL register
#define DWT_BR_110K     0   //!< UWB bit rate 110 kbits/s
#define DWT_BR_850K     1   //!< UWB bit rate 850 kbits/s
#define DWT_BR_6M8      2   //!< UWB bit rate 6.8 Mbits/s

//! constants for specifying the (Nominal) mean Pulse Repetition Frequency
//! These are defined for direct write (with a shift if necessary) to CHAN_CTRL and TX_FCTRL regs
#define DWT_PRF_16M     1   //!< UWB PRF 16 MHz
#define DWT_PRF_64M     2   //!< UWB PRF 64 MHz

//! constants for specifying Preamble Acquisition Chunk (PAC) Size in symbols
#define DWT_PAC8        0   //!< PAC  8 (recommended for RX of preamble length  128 and below
#define DWT_PAC16       1   //!< PAC 16 (recommended for RX of preamble length  256
#define DWT_PAC32       2   //!< PAC 32 (recommended for RX of preamble length  512
#define DWT_PAC64       3   //!< PAC 64 (recommended for RX of preamble length 1024 and up

//! constants for specifying TX Preamble length in symbols
//! These are defined to allow them be directly written into byte 2 of the TX_FCTRL register
//! (i.e. a four bit value destined for bits 20..18 but shifted left by 2 for byte alignment)
#define DWT_PLEN_4096   0x0C    //! Standard preamble length 4096 symbols
#define DWT_PLEN_2048   0x28    //! Non-standard preamble length 2048 symbols
#define DWT_PLEN_1536   0x18    //! Non-standard preamble length 1536 symbols
#define DWT_PLEN_1024   0x08    //! Standard preamble length 1024 symbols
#define DWT_PLEN_512    0x34    //! Non-standard preamble length 512 symbols
#define DWT_PLEN_256    0x24    //! Non-standard preamble length 256 symbols
#define DWT_PLEN_128    0x14    //! Non-standard preamble length 128 symbols
#define DWT_PLEN_64     0x04    //! Standard preamble length 64 symbols

#define DWT_SFDTOC_DEF              0x1041  // default SFD timeout value
#define DWT_PHRMODE_STD             0x0     // standard PHR mode
#define DWT_PHRMODE_EXT             0x3     // DW proprietary extended frames PHR mode

// Defined constants for "mode" bitmask parameter passed into dw1000_starttx() function.
typedef enum _dw1000_start_tx_modes_t {
    DWT_START_TX_IMMEDIATE = 1 << 0, 
    DWT_START_TX_DELAYED = 1 << 1, 
    DWT_RESPONSE_EXPECTED = 1 << 2
}dw1000_start_tx_modes_t;

// Defined constants for "mode" bitmask parameter passed into dw1000_start_rx() function.
typedef enum _dw1000_start_rx_modes_t {
    DWT_START_RX_IMMEDIATE = 1 << 0, 
    DWT_START_RX_DELAYED = 1 << 1,  // Set up delayed RX, if "late" error triggers, then the RX will be enabled immediately
    DWT_IDLE_ON_DLY_ERR = 1 << 2,   // If delayed RX failed due to "late" error then if this 
                                    // flag is set the RX will not be re-enabled immediately, and device will be in IDLE when function exits
    DWT_NO_SYNC_PTRS = 1 << 3       // Do not try to sync IC side and Host side buffer pointers when enabling RX. This is used to perform manual RX
                                    // re-enabling when receiving a frame in double buffer mode.
}dw1000_start_rx_modes_t;


//frame filtering configuration options
#define DWT_FF_NOTYPE_EN            0x000           // no frame types allowed (FF disabled)
#define DWT_FF_COORD_EN             0x002           // behave as coordinator (can receive frames with no dest address (PAN ID has to match))
#define DWT_FF_BEACON_EN            0x004           // beacon frames allowed
#define DWT_FF_DATA_EN              0x008           // data frames allowed
#define DWT_FF_ACK_EN               0x010           // ack frames allowed
#define DWT_FF_MAC_EN               0x020           // mac control frames allowed
#define DWT_FF_RSVD_EN              0x040           // reserved frame types allowed


//DW1000 SLEEP and WAKEUP configuration parameters
#define DWT_PRESRV_SLEEP 0x0100                      // PRES_SLEEP - on wakeup preserve sleep bit
#define DWT_LOADOPSET    0x0080                      // ONW_L64P - on wakeup load operating parameter set for 64 PSR
#define DWT_CONFIG       0x0040                      // ONW_LDC - on wakeup restore (load) the saved configurations (from AON array into HIF)
#define DWT_LOADEUI      0x0008                      // ONW_LEUI - on wakeup load EUI
#define DWT_RX_EN        0x0002                      // ONW_RX - on wakeup activate reception
#define DWT_TANDV        0x0001                      // ONW_RADC - on wakeup run ADC to sample temperature and voltage sensor values

#define DWT_XTAL_EN      0x10                       // keep XTAL running during sleep
#define DWT_WAKE_SLPCNT  0x8                        // wake up after sleep count
#define DWT_WAKE_CS      0x4                        // wake up on chip select
#define DWT_WAKE_WK      0x2                        // wake up on WAKEUP PIN
#define DWT_SLP_EN       0x1                        // enable sleep/deep sleep functionality

//DW1000 INIT configuration parameters
#define DWT_LOADUCODE     0x1
#define DWT_LOADNONE      0x0

//DW1000 OTP operating parameter set selection
#define DWT_OPSET_64LEN   0x0
#define DWT_OPSET_TIGHT   0x1
#define DWT_OPSET_DEFLT   0x2

// IEEE 802.15.4-2011 MAC Frame format
#define MAC_FFORMAT_FCTRL 0x0
#define MAC_FFORMAT_FCTRL_LEN 0x2
#define MAC_FFORMAT_FTYPE 0
#define MAC_FTYPE_BEACON  0x0
#define MAC_FTYPE_DATA    0x1
#define MAC_FTYPE_ACK     0x2
#define MAC_FTYPE_COMMAND 0x3


// TX/RX call-back data
typedef struct _dw1000_mac_cb_data_t {
    uint32_t status;      //initial value of register as ISR is entered
    uint16_t datalength;  //length of frame
    uint8_t  fctrl[2];    //frame control bytes
    uint8_t  rx_flags;    //RX frame flags, see above
} dw1000_mac_cb_data_t;

// Call-back type for all events
typedef void (*dw1000_mac_cb_t)(dw1000_dev_instance_t * inst, const dw1000_mac_cb_data_t *);

/*! ------------------------------------------------------------------------------------------------------------------
 * Structure typedef: dwt_config_t
 *
 * Structure for setting device configuration via dwt_configure() function
 *
 */
typedef struct _dwt_config_t {
    uint8_t chan ;           //!< channel number {1, 2, 3, 4, 5, 7 }
    uint8_t prf ;            //!< Pulse Repetition Frequency {DWT_PRF_16M or DWT_PRF_64M}
    uint8_t txPreambLength ; //!< DWT_PLEN_64..DWT_PLEN_4096
    uint8_t rxPAC ;          //!< Acquisition Chunk Size (Relates to RX preamble length)
    uint8_t txCode ;         //!< TX preamble code
    uint8_t rxCode ;         //!< RX preamble code
    uint8_t nsSFD ;          //!< Boolean should we use non-standard SFD for better performance
    uint8_t dataRate ;       //!< Data Rate {DWT_BR_110K, DWT_BR_850K or DWT_BR_6M8}
    uint8_t phrMode ;        //!< PHR mode {0x0 - standard DWT_PHRMODE_STD, 0x3 - extended frames DWT_PHRMODE_EXT}
    uint16_t sfdTO ;         //!< SFD timeout value (in symbols)
} dwt_config_t ;


/*
typedef struct _dw1000_mac_rxdiag_t {
    uint16_t      maxNoise ;          // LDE max value of noise
    uint16_t      firstPathAmp1 ;     // Amplitude at floor(index FP) + 1
    uint16_t      stdNoise ;          // Standard deviation of noise
    uint16_t      firstPathAmp2 ;     // Amplitude at floor(index FP) + 2
    uint16_t      firstPathAmp3 ;     // Amplitude at floor(index FP) + 3
    uint16_t      maxGrowthCIR ;      // Channel Impulse Response max growth CIR
    uint16_t      rxPreamCount ;      // Count of preamble symbols accumulated
    uint16_t      firstPath ;         // First path index (10.6 bits fixed point integer)
}dw1000_mac_rxdiag_t ;
*/

typedef struct _dw1000_mac_deviceentcnts_t{
    //all of the below are mapped to a 12-bit register in DW1000
    uint16_t PHE ;                    //number of received header errors
    uint16_t RSL ;                    //number of received frame sync loss events
    uint16_t CRCG ;                   //number of good CRC received frames
    uint16_t CRCB ;                   //number of bad CRC (CRC error) received frames
    uint16_t ARFE ;                   //number of address filter errors
    uint16_t OVER ;                   //number of receiver overflows (used in double buffer mode)
    uint16_t SFDTO ;                  //SFD timeouts
    uint16_t PTO ;                    //Preamble timeouts
    uint16_t RTO ;                    //RX frame wait timeouts
    uint16_t TXF ;                    //number of transmitted frames
    uint16_t HPW ;                    //half period warn
    uint16_t TXW ;                    //power up warn
} dw1000_mac_deviceentcnts_t ;

typedef struct _dw1000_mac_callbacks_t{
    void (* tx_complete_cb) (struct _dw1000_dev_instance_t *);
    void (* rx_complete_cb) (struct _dw1000_dev_instance_t *);
    void (* rx_timeout_cb) (struct _dw1000_dev_instance_t *);
    void (* rx_error_cb) (struct _dw1000_dev_instance_t *);
}dw1000_mac_callbacks_t;

dw1000_dev_status_t dw1000_mac_init(dw1000_dev_instance_t * inst, dwt_config_t *config);
void dw1000_tasks_init(dw1000_dev_instance_t * inst);
dw1000_dev_status_t dw1000_mac_framefilter(dw1000_dev_instance_t * inst, uint16_t enable);
dw1000_dev_status_t dw1000_write_tx(dw1000_dev_instance_t * inst,  uint8_t *txFrameBytes, uint16_t txBufferOffset, uint16_t txFrameLength);
dw1000_dev_status_t dw1000_start_tx(dw1000_dev_instance_t * inst);
dw1000_dev_status_t dw1000_set_delay_start(dw1000_dev_instance_t * inst, uint64_t delay);
dw1000_dev_status_t dw1000_set_wait4resp(dw1000_dev_instance_t * inst, bool enable);
dw1000_dev_status_t dw1000_start_rx(dw1000_dev_instance_t * inst);
void dw1000_write_tx_fctrl(dw1000_dev_instance_t * inst, uint16_t txFrameLength, uint16_t txBufferOffset, bool ranging);
void dw1000_read_rx(dw1000_dev_instance_t * inst, uint8_t *buffer, uint16_t rxBufferOffset, uint16_t length);
dw1000_dev_status_t dw1000_sync_rxbufptrs(dw1000_dev_instance_t * inst);
dw1000_dev_status_t dw1000_read_accdata(dw1000_dev_instance_t * inst, uint8_t *buffer, uint16_t len, uint16_t accOffset);
dw1000_dev_status_t dw1000_enable_autoack(dw1000_dev_instance_t * inst, uint8_t delay);

dw1000_dev_status_t dw1000_set_dblrxbuff(dw1000_dev_instance_t * inst, bool flag);
void dw1000_set_callbacks(dw1000_dev_instance_t * inst, dw1000_dev_cb_t cb_TxDone, dw1000_dev_cb_t cb_RxOk, dw1000_dev_cb_t cb_RxTo, dw1000_dev_cb_t cb_RxErr);
dw1000_dev_status_t dw1000_set_rx_timeout(dw1000_dev_instance_t * inst, uint16_t timeout);
    
#define dw1000_set_preamble_timeout(counts) dw1000_write_reg(inst, DRX_CONF_ID, DRX_PRETOC_OFFSET, counts, sizeof(uint16_t))
#define dw1000_read_rx(inst, buffer, rxBufferOffset, length) dw1000_read(inst, RX_BUFFER_ID,  rxBufferOffset, buffer,  length)
#define dw1000_set_panid(inst, pan_id) dw1000_write_reg(inst, PANADR_ID, PANADR_PAN_ID_OFFSET, pan_id, sizeof(uint16_t))
#define dw1000_set_address16(inst, shortAddress) dw1000_write_reg(inst ,PANADR_ID, PANADR_SHORT_ADDR_OFFSET, shortAddress, sizeof(uint16_t))
#define dw1000_set_eui(inst, eui64) dw1000_write_reg(inst, EUI_64_ID, EUI_64_OFFSET, eui64, EUI_64_LEN)
#define dw1000_get_eui(inst) (uint64_t) dw1000_read_reg(inst, EUI_64_ID, EUI_64_OFFSET, EUI_64_LEN)
#define dw1000_read_systime(inst) ((uint64_t) dw1000_read_reg(inst, SYS_TIME_ID, SYS_TIME_OFFSET, SYS_TIME_LEN) & 0xFFFFFFFFFF)
#define dw1000_read_systime_lo(inst) (uint32_t) dw1000_read_reg(inst, SYS_TIME_ID, SYS_TIME_OFFSET, sizeof(uint32_t))
#define dw1000_read_rxtime(inst) ((uint64_t) dw1000_read_reg(inst, RX_TIME_ID, RX_TIME_RX_STAMP_OFFSET, RX_TIME_RX_STAMP_LEN) & 0xFFFFFFFFFF)
#define dw1000_read_txtime(inst) ((uint64_t) dw1000_read_reg(inst, TX_TIME_ID, TX_TIME_TX_STAMP_OFFSET, TX_TIME_TX_STAMP_LEN) & 0xFFFFFFFFFF)
#define dw1000_read_rxtime_lo(inst) (uint32_t) dw1000_read_reg(inst, RX_TIME_ID, RX_TIME_RX_STAMP_OFFSET, sizeof(uint32_t))
#define dw1000_read_txtime_lo(inst) (uint32_t) dw1000_read_reg(inst, TX_TIME_ID, TX_TIME_TX_STAMP_OFFSET, sizeof(uint32_t))
#define dw1000_get_irqstatus(inst) ((uint8_t) dw1000_read_reg(inst, SYS_STATUS_ID, SYS_STATUS_OFFSET,sizeof(uint8_t)) & (uint8_t) SYS_STATUS_IRQS)

#ifdef __cplusplus
}
#endif
#endif /* _DW1000_MAC_H_ */
