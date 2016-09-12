/*
 * Copyright (C) 2010-2011 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mali_platform.c
 * Platform specific Mali driver functions for a default platform
 */

#include <linux/platform_device.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/pm.h>
#ifdef CONFIG_PM_RUNTIME
#include <linux/pm_runtime.h>
#endif
#include <asm/io.h>
#include <linux/mali/mali_utgard.h>

#include <mach/irqs.h>
#include <mach/hardware.h>
#include <mach/sci.h>
#include <mach/sci_glb_regs.h>

#include "mali_kernel_common.h"
#include "base.h"

static void mali_platform_device_release(struct device *device);
static void mali_platform_utilization(struct mali_gpu_utilization_data *data);

static struct resource mali_gpu_resources[] =
{
	MALI_GPU_RESOURCES_MALI400_MP4_PMU(SPRD_MALI_PHYS, IRQ_GPU_INT, IRQ_GPU_INT,
													IRQ_GPU_INT, IRQ_GPU_INT, IRQ_GPU_INT, IRQ_GPU_INT,
													IRQ_GPU_INT, IRQ_GPU_INT, IRQ_GPU_INT, IRQ_GPU_INT)
};

static struct mali_gpu_device_data mali_gpu_data =
{
	.shared_mem_size = ARCH_MALI_MEMORY_SIZE_DEFAULT,
	.utilization_interval = 300,
	.utilization_callback = mali_platform_utilization,
};

static struct platform_device mali_gpu_device =
{
	.name = MALI_GPU_NAME_UTGARD,
	.id = 0,
	.num_resources = ARRAY_SIZE(mali_gpu_resources),
	.resource = mali_gpu_resources,
	.dev.platform_data = &mali_gpu_data,
	.dev.release = mali_platform_device_release,
};

static struct clk* g_gpu_clock = NULL;

static int g_gpu_clock_on = 0;
static int g_gpu_power_on = 0;

int mali_platform_device_register(void)
{
	int err = -1;

	MALI_DEBUG_PRINT(4, ("mali_platform_device_register() called\n"));

	g_gpu_clock = clk_get(NULL, "clk_gpu");

	MALI_DEBUG_ASSERT(g_gpu_clock);

	if(!g_gpu_power_on)
	{
		g_gpu_power_on = 1;
		sci_glb_clr(REG_PMU_APB_PD_GPU_TOP_CFG, BIT_PD_GPU_TOP_FORCE_SHUTDOWN);
		msleep(5);
	}
	if(!g_gpu_clock_on)
	{
		g_gpu_clock_on = 1;
		clk_enable(g_gpu_clock);
		udelay(300);
	}

	err = platform_device_register(&mali_gpu_device);
	if (0 == err)
	{
#ifdef CONFIG_PM_RUNTIME
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
		pm_runtime_set_autosuspend_delay(&(mali_gpu_device.dev), 50);
		pm_runtime_use_autosuspend(&(mali_gpu_device.dev));
#endif
		pm_runtime_enable(&(mali_gpu_device.dev));
#endif

		return 0;
	}

	platform_device_unregister(&mali_gpu_device);

	if(g_gpu_clock_on)
	{
		g_gpu_clock_on = 0;
		clk_disable(g_gpu_clock);
	}
	if(g_gpu_power_on)
	{
		g_gpu_power_on = 0;
		sci_glb_set(REG_PMU_APB_PD_GPU_TOP_CFG, BIT_PD_GPU_TOP_FORCE_SHUTDOWN);
	}

	return err;
}

void mali_platform_device_unregister(void)
{
	MALI_DEBUG_PRINT(4, ("mali_platform_device_unregister() called\n"));

	platform_device_unregister(&mali_gpu_device);

	if(g_gpu_clock_on)
	{
		g_gpu_clock_on = 0;
		clk_disable(g_gpu_clock);
	}
	if(g_gpu_power_on)
	{
		g_gpu_power_on = 0;
		sci_glb_set(REG_PMU_APB_PD_GPU_TOP_CFG, BIT_PD_GPU_TOP_FORCE_SHUTDOWN);
	}
}

static void mali_platform_device_release(struct device *device)
{
	MALI_DEBUG_PRINT(4, ("mali_platform_device_release() called\n"));
}

void mali_platform_power_mode_change(int power_mode)
{
	#if 0
	switch(power_mode)
	{
	case 0://MALI_POWER_MODE_ON:
		if(!g_gpu_power_on)
		{
			g_gpu_power_on = 1;
			sci_glb_clr(REG_PMU_APB_PD_GPU_TOP_CFG, BIT_PD_GPU_TOP_FORCE_SHUTDOWN);
			msleep(5);
		}
		if(!g_gpu_clock_on)
		{
			g_gpu_clock_on = 1;
			clk_enable(g_gpu_clock);
			udelay(300);
		}
		break;
	case 1://MALI_POWER_MODE_LIGHT_SLEEP:
		if(g_gpu_clock_on)
		{
			g_gpu_clock_on = 0;
			clk_disable(g_gpu_clock);
		}
		break;
	case 2://MALI_POWER_MODE_DEEP_SLEEP:
		if(g_gpu_clock_on)
		{
			g_gpu_clock_on = 0;
			clk_disable(g_gpu_clock);
		}
		if(g_gpu_power_on)
		{
			g_gpu_power_on = 0;
			sci_glb_set(REG_PMU_APB_PD_GPU_TOP_CFG, BIT_PD_GPU_TOP_FORCE_SHUTDOWN);
		}
		break;
	};
	#endif
}

void mali_platform_utilization(struct mali_gpu_utilization_data *data)
{
	#if 0
	static int g_gpu_clock_div = 1;

	// if the loading ratio is greater then 90%, switch the clock to the maximum
	if(utilization >= (256*9/10))
	{
		g_gpu_clock_div = 1;
		sci_glb_write(REG_GLB_GEN2, BITS_CLK_GPU_AXI_DIV(g_gpu_clock_div-1), BITS_CLK_GPU_AXI_DIV(7));
		return;
	}

	if(utilization == 0)
	{
		utilization = 1;
	}

	// the absolute loading ratio is 1/g_gpu_clock_div * utilization/256
	// to keep the loading ratio under 70% at a certain level,
	// the absolute loading level is 1/(1/g_gpu_clock_div * utilization/256 / (7/10))
	g_gpu_clock_div = (256*7/10)*g_gpu_clock_div/utilization;

	if(g_gpu_clock_div < 1) g_gpu_clock_div = 1;
	if(g_gpu_clock_div > 8) g_gpu_clock_div = 8;

	sci_glb_write(REG_GLB_GEN2, BITS_CLK_GPU_AXI_DIV(g_gpu_clock_div-1), BITS_CLK_GPU_AXI_DIV(7));
	#endif
}

