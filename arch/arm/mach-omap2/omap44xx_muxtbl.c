/* arch/arm/mach-omap2/omap44xx_muxtbl.c
 *
 * Copyright (C) 2011 Samsung Electronics Co, Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <plat/io.h>

#include <mach/ctrl_module_pad_core_44xx.h>
#include <mach/ctrl_module_pad_wkup_44xx.h>

#include "control.h"
#include "mux.h"
#include "mux44xx.h"
#include "omap_muxtbl.h"
#include "omap44xx_muxtbl.h"

static struct omap_mux_partition *core_part;
static struct omap_mux_partition *wkup_part;

static int __init omap4_muxtbl_is_pbias_gpio(struct omap_board_mux *mux)
{
	struct omap_board_mux wkup_mux[] = {
		OMAP4_MUX(SIM_IO, OMAP_MUX_MODE3),
		OMAP4_MUX(SIM_CLK, OMAP_MUX_MODE3),
		OMAP4_MUX(SIM_RESET, OMAP_MUX_MODE3),
	};
	unsigned int i = ARRAY_SIZE(wkup_mux) - 1;

	do {
		if (wkup_mux[i].reg_offset == mux->reg_offset &&
		    (wkup_mux[i].value & mux->value))
			return 0;
	} while (i-- != 0);

	return -1;
}

static void __init omap4_muxtbl_set_pbias_gpio_pre(
		struct omap_mux_partition *partition,
		struct omap_board_mux *mux)
{
	unsigned int reg_val;

	if (partition != wkup_part || omap4_muxtbl_is_pbias_gpio(mux))
		return;

	reg_val = omap4_ctrl_pad_readl(OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);
	reg_val &= ~(OMAP4_USIM_PBIASLITE_PWRDNZ_MASK | OMAP4_USBC1_ICUSB_PWRDNZ_MASK);
	omap4_ctrl_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);

	reg_val = omap4_ctrl_wk_pad_readl(OMAP4_CTRL_MODULE_PAD_WKUP_CONTROL_USIMIO);
	reg_val &= ~(OMAP4_USIM_PBIASLITE_PWRDNZ_MASK);
	omap4_ctrl_wk_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_WKUP_CONTROL_USIMIO);

	reg_val = omap4_ctrl_pad_readl(OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);
	reg_val &= ~(OMAP4_USIM_PBIASLITE_HIZ_MODE_MASK);
	omap4_ctrl_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);
}

static void __init omap4_muxtbl_set_pbias_gpio_post(
		struct omap_mux_partition *partition,
		struct omap_board_mux *mux)
{
	unsigned int reg_val;

	if (partition != wkup_part || omap4_muxtbl_is_pbias_gpio(mux))
		return;

	reg_val = omap4_ctrl_pad_readl(OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_MMC1);
	reg_val &= ~(OMAP4_SDMMC1_DR0_SPEEDCTRL_MASK | OMAP4_USBC1_DR0_SPEEDCTRL_MASK);
	omap4_ctrl_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_MMC1);

	reg_val = omap4_ctrl_wk_pad_readl(OMAP4_CTRL_MODULE_PAD_WKUP_CONTROL_USIMIO);
	reg_val &= ~(OMAP4_PAD_USIM_CLK_LOW_MASK | OMAP4_PAD_USIM_RST_LOW_MASK);
	omap4_ctrl_wk_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_WKUP_CONTROL_USIMIO);

	reg_val = omap4_ctrl_pad_readl(OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);
	reg_val &= ~(OMAP4_USIM_PBIASLITE_VMODE_MASK | OMAP4_MMC1_PBIASLITE_VMODE_ERROR_MASK);
	omap4_ctrl_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);

	reg_val = omap4_ctrl_pad_readl(OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);
	reg_val |= (OMAP4_USIM_PBIASLITE_PWRDNZ_MASK | OMAP4_USBC1_ICUSB_PWRDNZ_MASK);
	omap4_ctrl_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);

	reg_val = omap4_ctrl_wk_pad_readl(OMAP4_CTRL_MODULE_PAD_WKUP_CONTROL_USIMIO);
	reg_val |= OMAP4_USIM_PWRDNZ_MASK;
	omap4_ctrl_wk_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_WKUP_CONTROL_USIMIO);

	reg_val = omap4_ctrl_pad_readl(OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);
	reg_val |= (OMAP4_USIM_PBIASLITE_PWRDNZ_MASK | OMAP4_USBC1_ICUSB_PWRDNZ_MASK);
	omap4_ctrl_pad_writel(reg_val, OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_PBIASLITE);
}

void __init omap4_muxtbl_init(void)
{
	core_part = omap_mux_get("core");
	wkup_part = omap_mux_get("wkup");
}

int __init omap4_muxtbl_add_mux(struct omap_muxtbl *muxtbl)
{
	struct omap_board_mux *mux = &muxtbl->mux;
	int wk_mux = muxtbl->domain;
	struct omap_mux_partition *partition;

	if (unlikely(wk_mux))
		partition = wkup_part;
	else
		partition = core_part;

	omap4_muxtbl_set_pbias_gpio_pre(partition, mux);

	omap_mux_write(partition, mux->value, mux->reg_offset);

	omap4_muxtbl_set_pbias_gpio_post(partition, mux);

	return 0;
}
