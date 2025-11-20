/**
  ******************************************************************************
  * @file    radio_board_if.c
  * @author  Matthijs Kooijman
  * @brief   This file provides an interface layer between MW and Radio Board
  ******************************************************************************
  * Copyright (c) 2022 STMicroelectronics.
  *
  * Revised BSD License - https://spdx.org/licenses/BSD-3-Clause.html
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions
  * are met:
  *
  *     1. Redistributions of source code must retain the above copyright notice,
  *        this list of conditions and the following disclaimer.
  *     2. Redistributions in binary form must reproduce the above copyright
  *        notice, this list of conditions and the following disclaimer in the
  *        documentation and/or other materials provided with the distribution.
  *     3. Neither the name of the copyright holder nor the names of its
  *        contributors may be used to endorse or promote products derived from this
  *        software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "radio_board_if.h"
#include "Arduino.h"

/* Board configuration --------------------------------------------------------*/

/* This section defines the board configuration values used. This
 * initially just hardcodes the values for the Nucleo WL55JC1 board, but
 * by using defines, these can potentially be overridden by
 * board-specifc defines on the commandline or in the variant files.
 */

// Is a TCXO present on the board?
#if !defined(LORAWAN_BOARD_HAS_TCXO)
  #define LORAWAN_BOARD_HAS_TCXO 1U
#endif

// Is circuitry for DCDC (SMPS) mode present on the board?
#if !defined(LORAWAN_BOARD_HAS_DCDC)
  #define LORAWAN_BOARD_HAS_DCDC 1U
#endif

// Maximum output power supported by output circuitry in LP mode
#if !defined(LORAWAN_RFO_LP_MAX_POWER )
  #define LORAWAN_RFO_LP_MAX_POWER 15 /* dBm */
#endif

// Maximum output power supported by output circuitry in HP mode
#if !defined(LORAWAN_RFO_HP_MAX_POWER )
  #define LORAWAN_RFO_HP_MAX_POWER 22 /* dBm */
#endif

// Supported TX modes (LP/HP or both)
#if !defined(LORAWAN_TX_CONFIG)
  #define LORAWAN_TX_CONFIG RBI_CONF_RFO_HP
#endif


/* Exported functions --------------------------------------------------------*/
int32_t RBI_Init(void)
{
    pinMode(PA4,OUTPUT);
    pinMode(PA5,OUTPUT);
    digitalWrite(PA4, LOW);
    digitalWrite(PA5, LOW);

  return 0;
}

int32_t RBI_DeInit(void)
{
  return 0;
}

int32_t RBI_ConfigRFSwitch(RBI_Switch_TypeDef Config)
{
  switch(Config)
  {
    case RBI_SWITCH_OFF : 
    digitalWrite(PA4, LOW);
    digitalWrite(PA5, LOW);
    break;

    case RBI_SWITCH_RX : 
    digitalWrite(PA4, HIGH);
    digitalWrite(PA5, LOW);
    break;

    case RBI_SWITCH_RFO_LP : 
    digitalWrite(PA4, LOW);
    digitalWrite(PA5, HIGH);
    break;

    case RBI_SWITCH_RFO_HP : 
    digitalWrite(PA4, LOW);
    digitalWrite(PA5, HIGH);
    break;
    
    
  }
  return 0;
}

int32_t RBI_GetTxConfig(void)
{
  return LORAWAN_TX_CONFIG;
}

int32_t RBI_IsTCXO(void)
{
  return LORAWAN_BOARD_HAS_TCXO;
}

int32_t RBI_IsDCDC(void)
{
  return LORAWAN_BOARD_HAS_DCDC;
}

int32_t RBI_GetRFOMaxPowerConfig(RBI_RFOMaxPowerConfig_TypeDef Config)
{
  return LORAWAN_RFO_HP_MAX_POWER;
}
