/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef METAL__DRIVERS__SIFIVE_GPIO_SWITCHES_H
#define METAL__DRIVERS__SIFIVE_GPIO_SWITCHES_H

{% include 'template_comment.h' %}

#include <metal/generated/switch.h>
#include <metal/gpio.h>
#include <metal/interrupt.h>

static const struct dt_switch_data {
	char *label;
	struct metal_gpio gpio;
	struct metal_interrupt *interrupt_parent;
	uint32_t interrupt_id;
} dt_switch_data[__METAL_DT_NUM_SWITCHES] = {
	{% for sw in switches %}
	{
		.label = "{{ sw.label }}",

	    .gpio = {{ sw.gpio.id }},

		{% if defined(sw.interrupt_parent) %}
			.interrupt_parent = (struct metal_interrupt *) = {{ metal_interrupt(sw.interrupt_parent.id) }},
			.interrupt_id = {{ sw.interrupts[0] }},
		{% else %}
			.interrupt_parent = NULL,
		{% endif %}
	},
	{% endfor %}
};

#endif