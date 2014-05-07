/* am7xxx - communication with AM7xxx based USB Pico Projectors and DPFs
 *
 * Copyright (C) 2014  Antonio Ospite <ao2@ao2.it>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <time.h>

#include "tools.h"

/**
 * Sleep for a period expressed in milliseconds
 *
 * @param[in] msecs Time to sleep in milliseconds
 *
 * @return 0 on success, -1 on error
 */
int msleep(unsigned long msecs)
{
	struct timespec delay;
	int ret;

	delay.tv_sec = msecs / 1000;
	delay.tv_nsec = (msecs % 1000) * 1000000;
	while (1) {
		ret = nanosleep(&delay, &delay);
		if (ret == -1 && errno == EINTR)
			continue;
		break;
	}
	if (ret == -1)
		return ret;

	return 0;
}
