#ifndef _CONF_WINC_H_
#define _CONF_WINC_H_

/* Network Device Selection */
//#if defined(_WINC1500_)
//#elif defined(_WINC3400_)
//#else
//  #error "Select one of Network device above!!" #__FILE__ #__LINE__
//#endif

#include "kb_common_header.h"
/** Debug Options */
#if defined(KB_LIBRARY) // TODO: Delete this line..
	#ifdef KB_MSG_BASE
		#undef KB_MSG_BASE
		#define KB_MSG_BASE "WINC1500"
	#endif
	#define CONF_WINC_DEBUG                     KB_DEBUG_MSG
	#define CONF_WINC_PRINTF                    KB_DEBUG_MSG
#else
	#include <stdio.h>
	#define CONF_WINC_DEBUG                     (1)
	#define CONF_WINC_PRINTF                    printf
#endif
/**
 * @defgroup MCUSelect
 * Selec only one MCU in this group
 * @{
 */
//#define _ATMEGA128_
//#define _ATMEGA328PB_

/**
 * @}
 */

/* Header selection */
#if defined(KB_LIBRARY)
	#include "conf_winc_kb_lib.h"
#elif defined(_ATMEGA128_)
	#include "conf_winc_atmega128.h"
#elif defined(_ATMEGA328PB_)
	#include "conf_winc_atmega328pb.h"
#elif defined(__SAMD21J18A__)
	#include "conf_winc_SAMD21J18A.h"
#else
	#error "Select one of device above!!" #__FILE__ #__LINE__
#endif

#endif /* _CONF_WINC_H_ */
