/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    LPC17xx/hal_lld.c
 * @brief   LPC17xx HAL subsystem low level driver source.
 *
 * @addtogroup HAL
 * @{
 */

#include "ch.h"
#include "hal.h"

/**
 * @brief   Register missing in NXP header file.
 */
#define FLASHCFG (*((volatile uint32_t *)0x400FC000))

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level HAL driver initialization.
 *
 * @notapi
 */
void hal_lld_init(void) {

  /* SysTick initialization using the system clock.*/
  nvicSetSystemHandlerPriority(HANDLER_SYSTICK, CORTEX_PRIORITY_SYSTICK);
  SysTick->LOAD = LPC17xx_SYSCLK / CH_FREQUENCY - 1;
  SysTick->VAL = 0;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                  SysTick_CTRL_ENABLE_Msk |
                  SysTick_CTRL_TICKINT_Msk;
}

/**
 * @brief   LPC17xx clocks and PLL initialization.
 * @note    All the involved constants come from the file @p board.h.
 * @note    This function must be invoked only after the system reset.
 *
 * @special
 */
void LPC17xx_clock_init(void) {

  /* Flash wait states setting, the code takes care to not touch TBD bits.*/
  FLASHCFG = (FLASHCFG & ~(0xf << 12)) | LPC17xx_FLASHCFG_FLASHTIM<<12;

  /* System oscillator initialization if required.*/
#if LPC17xx_PLLCLK_SOURCE == SYSPLLCLKSEL_SYSOSC
  LPC_SYSCON->SYSSCS = LPC17xx_SYSOSCCTRL<< 4 | (1 << 5);
  while ((LPC_SYSCON->SYSSCS & (1 << 6)) ==0 )
    __NOP();                                    /* Stabilization delay.     */
#endif /* LPC17xx_PLLCLK_SOURCE == SYSPLLCLKSEL_SYSOSC */

  LPC_SYSCON->CLKSRCSEL = LPC17xx_PLLCLK_SOURCE;    /* Select Clock Source for PLL0       */
  LPC_SYSCON->CCLKCFG   = LPC17xx_SYSCPUCLK_DIV;    /* Setup Clock Divider                */
  LPC_SYSCON->PCLKSEL0  = PCLKSEL0_Val;             /* Peripheral Clock Selection         */
  LPC_SYSCON->PCLKSEL1  = PCLKSEL1_Val;

#if LPC17xx_MAINCLK_SOURCE == SYSMAINCLKSEL_PLLOUT
  /* PLL initialization if required.*/

  LPC_SYSCON->SYSPLLCFG   = LPC17xx_SYSPLLCTRL_MSELO | (LPC17xx_SYSPLLCTRL_NSELO<< 16);
  LPC_SYSCON->SYSPLLCON   = 0x01;             /* PLL0 Enable                        */
  LPC_SYSCON->SYSPLLFEED  = 0xAA;
  LPC_SYSCON->SYSPLLFEED  = 0x55;
  while (!(LPC_SYSCON->SYSPLLSTAT & (1<<26)));/* Wait for PLOCK0                    */

  LPC_SYSCON->SYSPLLCON   = 0x03;             /* PLL0 Enable & Connect              */
  LPC_SYSCON->SYSPLLFEED  = 0xAA;
  LPC_SYSCON->SYSPLLFEED  = 0x55;
#endif


  /* Memory remapping, vectors always in ROM.*/
//  LPC_SYSCON->SYSMEMREMAP = 2;
}

/** @} */
