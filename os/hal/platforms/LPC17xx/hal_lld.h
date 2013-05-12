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
 * @file    LPC17xx/hal_lld.h
 * @brief   HAL subsystem low level driver header template.
 *
 * @addtogroup HAL
 * @{
 */

#ifndef _HAL_LLD_H_
#define _HAL_LLD_H_

#include "LPC17xx.h"
#include "nvic.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @brief   Defines the support for realtime counters in the HAL.
 */
#define HAL_IMPLEMENTS_COUNTERS FALSE

/**
 * @brief   Platform name.
 */
#define PLATFORM_NAME           "LPC17xx"

#define IRCOSCCLK               4000000     /**< High speed internal clock. */
#define SYSOSCCLK               12000000    /**< oscillator external clock. */
#define WDGOSCCLK               1600000     /**< Watchdog internal clock.   */
#define RTCOSCCLK               32000       /**< Real Time Clock oscillator */

#define SYSCLKSEL_IRCOSC        0           /**< Internal RC oscillator
                                                 clock source.              */
#define SYSCLKSEL_SYSOSC        1           /**< System oscillator clock
                                                 source.                    */
#define SYSCLKSEL_RTCOSC        2           /**< RTC oscillator clock
                                                 source.                    */

#define SYSMAINCLKSEL_PLLIN     0           /**< Clock source is PLLIN.     */
#define SYSMAINCLKSEL_PLLOUT    1           /**< Clock source is PLLOUT.    */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   System PLL clock source.
 */
#if !defined(LPC17xx_PLLCLK_SOURCE) || defined(__DOXYGEN__)
#define LPC17xx_PLLCLK_SOURCE               SYSCLKSEL_SYSOSC
#endif

/**
 * @brief   System PLL multiplier.
 * @note    The value must be in the 6..512 range and the final frequency
 *          must not exceed the CCO ratings.
 */
#if !defined(LPC17xx_SYSPLL_MUL) || defined(__DOXYGEN__)
#define LPC17xx_SYSPLL_MUL                  25
#endif

/**
 * @brief   System PLL Pre-divider.
 * @note    The value must be in the 1..32 range and the final frequency
 *          must not exceed the CCO ratings.
 */
#if !defined(LPC17xx_SYSPLL_DIV) || defined(__DOXYGEN__)
#define LPC17xx_SYSPLL_DIV                  2
#endif


/**
 * @brief   System main clock source.
 */
#if !defined(LPC17xx_MAINCLK_SOURCE) || defined(__DOXYGEN__)
#define LPC17xx_MAINCLK_SOURCE              SYSMAINCLKSEL_PLLOUT
#endif

/**
 * @brief   CPU clock divider.
 * @note    The value must be chosen between (1...255).
 */
#if !defined(LPC17xx_SYSCPUCLK_DIV) || defined(__DOXYGEN__)
#define LPC17xx_SYSCPUCLK_DIV               3
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/**
 * @brief   Calculated SYSOSCCTRL setting.
 */
#if (SYSOSCCLK < 18000000) || defined(__DOXYGEN__)
#define LPC17xx_SYSOSCCTRL      0
#else
#define LPC17xx_SYSOSCCTRL      1
#endif

/**
 * @brief   PLL input clock frequency.
 */
#if (LPC17xx_PLLCLK_SOURCE == SYSPLLCLKSEL_SYSOSC) || defined(__DOXYGEN__)
#define LPC17xx_SYSPLLCLKIN     SYSOSCCLK
#elif LPC17xx_PLLCLK_SOURCE == SYSPLLCLKSEL_IRCOSC
#define LPC17xx_SYSPLLCLKIN     IRCOSCCLK
#elif LPC17xx_PLLCLK_SOURCE == SYSPLLCLKSEL_RTCOSC
#define LPC17xx_SYSPLLCLKIN     RTCOSCCLK
#else
#error "invalid LPC17xx_PLLCLK_SOURCE clock source specified"
#endif

/**
 * @brief   MSEL mask in SYSPLLCTRL register.
 */
#if (LPC17xx_SYSPLL_MUL >= 1) && (LPC17xx_SYSPLL_MUL <= 512) ||              \
    defined(__DOXYGEN__)
#define LPC17xx_SYSPLLCTRL_MSELO (LPC17xx_SYSPLL_MUL - 1)
#else
#error "LPC17xx_SYSPLL_MUL out of range (1...512)"
#endif

/**
 * @brief   PSEL mask in SYSPLLCTRL register.
 */
#if (LPC17xx_SYSPLL_DIV >= 2) && (LPC17xx_SYSPLL_DIV <= 32)|| defined(__DOXYGEN__)
#define LPC17xx_SYSPLLCTRL_NSEL0 (LPC17xx_SYSPLL_DIV -1)
#else
#error "LPC17xx_SYSPLL_DIV out of range (2...22)"
#endif

/**
 * @brief   CCP frequency.
 */
#define  LPC17xx_SYSPLLCCO   ((2 * LPC17xx_SYSPLLCLKIN * LPC17xx_SYSPLL_MUL) /    \
                              LPC17xx_SYSPLL_DIV)

#if (LPC17xx_SYSPLLCCO < 275000000) || (LPC17xx_SYSPLLCCO > 550000000)
#error "FCCO frequency out of the acceptable range (275...550)"
#endif

/**
 * @brief   PLL output clock frequency.
 */
#define  LPC17xx_SYSPLLCLKOUT   (LPC17xx_SYSPLLCCO / LPC17xx_SYSPLL_DIV)

#if (LPC17xx_MAINCLK_SOURCE == SYSMAINCLKSEL_IRCOSC) || defined(__DOXYGEN__)
#define LPC17xx_MAINCLK     IRCOSCCLK
#elif LPC17xx_MAINCLK_SOURCE == SYSMAINCLKSEL_PLLIN
#define LPC17xx_MAINCLK     LPC17xx_SYSPLLCLKIN
#elif LPC17xx_MAINCLK_SOURCE == SYSMAINCLKSEL_WDGOSC
#define LPC17xx_MAINCLK     WDGOSCCLK
#elif LPC17xx_MAINCLK_SOURCE == SYSMAINCLKSEL_PLLOUT
#define LPC17xx_MAINCLK     LPC17xx_SYSPLLCLKOUT
#else
#error "invalid LPC17xx_MAINCLK_SOURCE clock source specified"
#endif

/**
 * @brief   AHB clock.
 */
#define  LPC17xx_SYSCLK     (LPC17xx_MAINCLK / LPC17xx_SYSCPUCLK_DIV)
#if LPC17xx_SYSCLK > 120000000
#error "CPU clock frequency out of the acceptable range (120MHz max)"
#endif

/**
 * @brief   Flash wait states.
 */
#if (LPC17xx_SYSCLK <= 20000000) || defined(__DOXYGEN__)
#define LPC17xx_FLASHCFG_FLASHTIM   0
#elif LPC17xx_SYSCLK <= 40000000
#define LPC17xx_FLASHCFG_FLASHTIM   1
#elif LPC17xx_SYSCLK <= 60000000
#define LPC17xx_FLASHCFG_FLASHTIM   2
#elif LPC17xx_SYSCLK <= 80000000
#define LPC17xx_FLASHCFG_FLASHTIM   3
#elif LPC17xx_SYSCLK <= 100000000
#define LPC17xx_FLASHCFG_FLASHTIM   4
#else
#define LPC17xx_FLASHCFG_FLASHTIM   5
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void hal_lld_init(void);
  void LPC17xx_clock_init(void);
#ifdef __cplusplus
}
#endif

#endif /* _HAL_LLD_H_ */

/** @} */
