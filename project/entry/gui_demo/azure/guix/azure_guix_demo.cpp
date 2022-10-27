#include <linux/kconfig.h>
#include <stdio.h>
#include <gx_api.h>

#include "azure_guix_demo.h"

#if defined(CONFIG_AZURE_GUIX_SIMPLE)
#include "simple/guix_simple.h"
#endif

#if defined(CONFIG_AZURE_GUIX_INDUSTRIAL)
#include "industrial/guix_industrial.h"
#endif

#if defined(CONFIG_AZURE_GUIX_CALCULATOR)
#include "calculator/guix_calculator.h"
#endif

#if defined(CONFIG_AZURE_GUIX_WIDGET_TYPES)
#include "widget_types/guix_widget_types.h"
#endif

#if defined(CONFIG_AZURE_GUIX_WASHING_MACHINE)
#include "washing_machine/guix_washing_machine.h"
#endif

#if defined(CONFIG_AZURE_GUIX_THERMOSTAT)
#include "thermostat/guix_thermostat.h"
#endif

#if defined(CONFIG_AZURE_GUIX_SPRITE)
#include "sprite/guix_sprite.h"
#endif

#if defined(CONFIG_AZURE_GUIX_SMART_WATCH)
#include "smart_watch/guix_smart_watch.h"
#endif

#if defined(CONFIG_AZURE_GUIX_SHAPES)
#include "shapes/guix_shapes.h"
#endif

#if defined(CONFIG_AZURE_GUIX_MEDICAL)
#include "medical/guix_medical.h"
#endif

#if defined(CONFIG_AZURE_GUIX_KEYBOARD)
#include "keyboard/guix_keyboard.h"
#endif

#if defined(CONFIG_AZURE_GUIX_HOME_AUTOMATION)
#include "home_automation/guix_home_automation.h"
#endif

#if defined(CONFIG_AZURE_GUIX_CAR_INFOTAINMENT)
#include "car_infotainment/guix_car_infotainment.h"
#endif

#if defined(CONFIG_AZURE_GUIX_WIDGET_ANIMATION)
#include "widget_animation/guix_widget_animation.h"
#endif

int azure_guix_demo_init(int argc, char *argv[])
{
#if defined(CONFIG_AZURE_GUIX_SIMPLE)
    return azure_guix_simple_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_INDUSTRIAL)
    return azure_guix_industrial_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_CALCULATOR)
    return azure_guix_calculator_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_WIDGET_TYPES)
    return azure_guix_widget_types_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_WASHING_MACHINE)
    return azure_guix_washing_machine_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_THERMOSTAT)
    return azure_guix_thermostat_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_SPRITE)
    return azure_guix_sprite_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_SMART_WATCH)
    return azure_guix_smart_watch_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_SHAPES)
    return azure_guix_shapes_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_MEDICAL)
    return azure_guix_medical_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_KEYBOARD)
    return azure_guix_keyboard_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_HOME_AUTOMATION)
    return azure_guix_home_automation_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_CAR_INFOTAINMENT)
    return azure_guix_car_infotainment_demo_init(argc, argv);
#elif defined(CONFIG_AZURE_GUIX_WIDGET_ANIMATION)
    return azure_guix_widget_animation_demo_init(argc, argv);
#endif

    return -1;
}

int azure_guix_demo_exit(void)
{
#if defined(CONFIG_AZURE_GUIX_SIMPLE)
    return azure_guix_simple_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_INDUSTRIAL)
    return azure_guix_industrial_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_CALCULATOR)
    return azure_guix_calculator_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_WIDGET_TYPES)
    return azure_guix_widget_types_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_WASHING_MACHINE)
    return azure_guix_washing_machine_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_THERMOSTAT)
    return azure_guix_thermostat_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_SPRITE)
    return azure_guix_sprite_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_SMART_WATCH)
    return azure_guix_smart_watch_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_SHAPES)
    return azure_guix_shapes_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_MEDICAL)
    return azure_guix_medical_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_KEYBOARD)
    return azure_guix_keyboard_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_HOME_AUTOMATION)
    return azure_guix_home_automation_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_CAR_INFOTAINMENT)
    return azure_guix_car_infotainment_demo_exit();
#elif defined(CONFIG_AZURE_GUIX_WIDGET_ANIMATION)
    return azure_guix_widget_animation_demo_exit();
#endif

    return -1;
}
