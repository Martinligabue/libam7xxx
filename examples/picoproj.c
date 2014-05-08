/* picoproj - test program for libam7xxx
 *
 * Copyright (C) 2012-2014  Antonio Ospite <ao2@ao2.it>
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

/**
 * @example examples/picoproj.c
 * A minimal example to show how to use libam7xxx to display a static image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "am7xxx.h"

static void usage(char *name)
{
	printf("usage: %s [OPTIONS]\n\n", name);
	printf("OPTIONS:\n");
	printf("\t-d <index>\t\tthe device index (default is 0)\n");
	printf("\t-f <filename>\t\tthe image file to upload\n");
	printf("\t-F <format>\t\tthe image format to use (default is JPEG)\n");
	printf("\t\t\t\tSUPPORTED FORMATS:\n");
	printf("\t\t\t\t\t1 - JPEG\n");
	printf("\t\t\t\t\t2 - NV12\n");
	printf("\t-l <log level>\t\tthe verbosity level of libam7xxx output (0-5)\n");
	printf("\t-p <power mode>\t\tthe power mode of device, between %d (off) and %d (turbo)\n",
	       AM7XXX_POWER_OFF, AM7XXX_POWER_TURBO);
	printf("\t\t\t\tWARNING: Level 2 and greater require the master AND\n");
	printf("\t\t\t\t         the slave connector to be plugged in.\n");
	printf("\t-z <zoom mode>\t\tthe display zoom mode, between %d (original) and %d (tele)\n",
	       AM7XXX_ZOOM_ORIGINAL, AM7XXX_ZOOM_TELE);
	printf("\t-W <image width>\tthe width of the image to upload\n");
	printf("\t-H <image height>\tthe height of the image to upload\n");
	printf("\t-h \t\t\tthis help message\n");
	printf("\n\nEXAMPLE OF USE:\n");
	printf("\t%s -f file.jpg -F 1 -l 5 -W 800 -H 480\n", name);
}

int main(int argc, char *argv[])
{
	int ret;
	int opt;

	char filename[FILENAME_MAX] = {0};
	FILE *image_fp;
	struct stat st;
	am7xxx_context *ctx;
	am7xxx_device *dev;
	int log_level = AM7XXX_LOG_INFO;
	int device_index = 0;
	int power_mode = AM7XXX_POWER_LOW;
	int zoom = AM7XXX_ZOOM_ORIGINAL;
	int format = AM7XXX_IMAGE_FORMAT_JPEG;
	int width = 800;
	int height = 480;
	unsigned char *image;
	off_t size;
	am7xxx_device_info device_info;

	while ((opt = getopt(argc, argv, "d:f:F:l:p:z:W:H:h")) != -1) {
		switch (opt) {
		case 'd':
			device_index = atoi(optarg);
			if (device_index < 0) {
				fprintf(stderr, "Unsupported device index\n");
				ret = -EINVAL;
				goto out;
			}
			break;
		case 'f':
			if (filename[0] != '\0')
				fprintf(stderr, "Warning: image file already specified\n");
			strncpy(filename, optarg, FILENAME_MAX);
			break;
		case 'F':
			format = atoi(optarg);
			switch(format) {
			case AM7XXX_IMAGE_FORMAT_JPEG:
				fprintf(stdout, "JPEG format\n");
				break;
			case AM7XXX_IMAGE_FORMAT_NV12:
				fprintf(stdout, "NV12 format\n");
				break;
			default:
				fprintf(stderr, "Unsupported format\n");
				ret = -EINVAL;
				goto out;
			}
			break;
		case 'l':
			log_level = atoi(optarg);
			if (log_level < AM7XXX_LOG_FATAL || log_level > AM7XXX_LOG_TRACE) {
				fprintf(stderr, "Unsupported log level, falling back to AM7XXX_LOG_ERROR\n");
				log_level = AM7XXX_LOG_ERROR;
			}
			break;
		case 'p':
			power_mode = atoi(optarg);
			switch(power_mode) {
			case AM7XXX_POWER_OFF:
			case AM7XXX_POWER_LOW:
			case AM7XXX_POWER_MIDDLE:
			case AM7XXX_POWER_HIGH:
			case AM7XXX_POWER_TURBO:
				fprintf(stdout, "Power mode: %d\n", power_mode);
				break;
			default:
				fprintf(stderr, "Invalid power mode value, must be between %d and %d\n",
					AM7XXX_POWER_OFF, AM7XXX_POWER_TURBO);
				ret = -EINVAL;
				goto out;
			}
			break;
		case 'z':
			zoom = atoi(optarg);
			switch(zoom) {
			case AM7XXX_ZOOM_ORIGINAL:
			case AM7XXX_ZOOM_H:
			case AM7XXX_ZOOM_H_V:
			case AM7XXX_ZOOM_TEST:
			case AM7XXX_ZOOM_TELE:
				fprintf(stdout, "Zoom: %d\n", zoom);
				break;
			default:
				fprintf(stderr, "Invalid zoom mode value, must be between %d and %d\n",
					AM7XXX_ZOOM_ORIGINAL, AM7XXX_ZOOM_TELE);
				ret = -EINVAL;
				goto out;
			}
			break;
		case 'W':
			width = atoi(optarg);
			if (width < 0) {
				fprintf(stderr, "Unsupported width\n");
				ret = -EINVAL;
				goto out;
			}
			break;
		case 'H':
			height = atoi(optarg);
			if (height < 0) {
				fprintf(stderr, "Unsupported height\n");
				ret = -EINVAL;
				goto out;
			}
			break;
		case 'h':
			usage(argv[0]);
			ret = 0;
			goto out;
		default: /* '?' */
			usage(argv[0]);
			ret = -EINVAL;
			goto out;
		}
	}

	if (filename[0] == '\0') {
		fprintf(stderr, "An image file MUST be specified with the -f option.\n\n");
		usage(argv[0]);
		ret = -EINVAL;
		goto out;
	}

	image_fp = fopen(filename, "rb");
	if (image_fp == NULL) {
		perror("fopen");
		ret = -EINVAL;
		goto out;
	}
	ret = fstat(fileno(image_fp), &st);
	if (ret < 0) {
		perror("fstat");
		goto out_close_image_fp;
	}
	size = st.st_size;

	image = malloc(size * sizeof(unsigned char));
	if (image == NULL) {
		perror("malloc");
		ret = -ENOMEM;
		goto out_close_image_fp;
	}

	ret = (int)fread(image, size, 1, image_fp);
	if (ret != 1) {
		if (feof(image_fp))
			fprintf(stderr, "Unexpected end of file.\n");
		else if (ferror(image_fp))
			perror("fread");
		else
			fprintf(stderr, "Unexpected error condition.\n");

		if (ret >= 0)
			ret = -EINVAL;
		goto out_free_image;
	}

	ret = am7xxx_init(&ctx);
	if (ret < 0) {
		perror("am7xxx_init");
		goto out_free_image;
	}

	am7xxx_set_log_level(ctx, log_level);

	ret = am7xxx_open_device(ctx, &dev, 0);
	if (ret < 0) {
		perror("am7xxx_open_device");
		goto cleanup;
	}

	ret = am7xxx_close_device(dev);
	if (ret < 0) {
		perror("am7xxx_close_device");
		goto cleanup;
	}

	ret = am7xxx_open_device(ctx, &dev, device_index);
	if (ret < 0) {
		perror("am7xxx_open_device");
		goto cleanup;
	}

	ret = am7xxx_get_device_info(dev, &device_info);
	if (ret < 0) {
		perror("am7xxx_get_device_info");
		goto cleanup;
	}
	printf("Native resolution: %dx%d\n",
	       device_info.native_width, device_info.native_height);

	ret = am7xxx_set_zoom_mode(dev, zoom);
	if (ret < 0) {
		perror("am7xxx_set_zoom_mode");
		goto cleanup;
	}

	ret = am7xxx_set_power_mode(dev, power_mode);
	if (ret < 0) {
		perror("am7xxx_set_power_mode");
		goto cleanup;
	}

	/* When setting AM7XXX_ZOOM_TEST don't display the actual image */
	if (zoom == AM7XXX_ZOOM_TEST) {
		printf("AM7XXX_ZOOM_TEST requested, not sending actual image.\n");
		goto cleanup;
	}

	if ((unsigned int)width > device_info.native_width ||
	    (unsigned int)height > device_info.native_height)
		fprintf(stderr,
			"WARNING: image is %dx%d, not fitting the native resolution, it may be displayed wrongly!\n",
			width, height);

	ret = am7xxx_send_image(dev, format, width, height, image, (unsigned int)size);
	if (ret < 0) {
		perror("am7xxx_send_image");
		goto cleanup;
	}

	ret = 0;

cleanup:
	am7xxx_shutdown(ctx);

out_free_image:
	free(image);

out_close_image_fp:
	if (fclose(image_fp) == EOF)
		perror("fclose");

out:
	return ret;
}
