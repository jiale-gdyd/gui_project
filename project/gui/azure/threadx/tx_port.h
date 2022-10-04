#ifndef TX_PORT_H
#define TX_PORT_H

#include <linux/kconfig.h>

#if defined(CONFIG_CORTEX_A7)
//#include "port/tx_cortex_a7_port.h"
#include "port/tx_linux_gnu_port.h"
#elif defined(CONFIG_LINUX_GNU)
#include "port/tx_linux_gnu_port.h"
#endif

#endif
