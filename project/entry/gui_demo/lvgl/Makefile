ifeq ($(CONFIG_LVGL_V7),y)
obj-y += printer/
else
obj-$(CONFIG_LVGL_MUSIC_TEST) += music/
obj-$(CONFIG_LVGL_STRESS_TEST) += stress/
obj-$(CONFIG_LVGL_WIDGET_TEST) += widget/
obj-$(CONFIG_LVGL_BENCHMARK_TEST) += benchmark/
obj-$(CONFIG_LVGL_KEYPAD_ENCODER_TEST) += keypad/
endif

obj-y += lvgl_demo.o
