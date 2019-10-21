#ifndef BEEP_IOCTL_H_
#define BEEP_IOCTL_H_

#include <linux/ioctl.h>

#define BEEP_TYPE 'B'

#define BEEP_SET_HZ  _IOW(BEEP_TYPE, 0, int)
#define BEEP_MUTE    _IO(BEEP_TYPE, 1)

#endif
