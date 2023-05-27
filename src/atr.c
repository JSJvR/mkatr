/*
 *  Copyright (C) 2023 Daniel Serpell
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>
 */
/*
 * Load ATR files.
 */

#include "atr.h"
#include "msg.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Load disk image from file
struct atr_image *load_atr_image(const char *file_name)
{
    FILE *f = fopen(file_name, "rb");
    if(!f)
    {
        show_error("can´t open disk image '%s': %s", file_name, strerror(errno));
        return 0;
    }

    // Get header
    uint8_t hdr[16];
    if(1 != fread(hdr, 16, 1, f))
    {
        show_error("%s: can´t read ATR header", file_name);
        fclose(f);
        return 0;
    }
    if(hdr[0] != 0x96 || hdr[1] != 0x02)
    {
        show_error("%s: not an ATR image", file_name);
        fclose(f);
        return 0;
    }
    unsigned ssz = hdr[4] | (hdr[5] << 8);
    if(ssz != 128 && ssz != 256)
    {
        show_error("%s: unsupported ATR sector size (%d)", file_name, ssz);
        fclose(f);
        return 0;
    }
    unsigned isz = (hdr[2] << 4) | (hdr[3] << 12) | (hdr[6] << 20);
    // Some images store full size fo the first 3 sectors, others store
    // 128 bytes for those:
    unsigned pad_size = isz % ssz == 0 ? 0 : (ssz - 128) * 3;
    unsigned num_sectors = (isz + pad_size) / ssz;
    if(isz >= 0x1000000 || num_sectors * ssz - pad_size != isz)
    {
        // If image size is invalid, assume sector padding:
        pad_size = 3 * (ssz - 128);
        num_sectors = (isz + pad_size) / ssz;
        if(num_sectors < 3)
            show_error("%s: invalid ATR image size (%d), too small.", file_name, isz);
        show_msg("%s: invalid ATR image size (%d), rounding down to (%d)", file_name, isz,
                 num_sectors * ssz - pad_size);
    }
    // Allocate new storage
    uint8_t *data = malloc(ssz * num_sectors);
    // Read 3 first sectors
    for(int i = 0; i < num_sectors; i++)
    {
        if(1 != fread(data + ssz * i, (i < 3 && pad_size) ? 128 : ssz, 1, f))
        {
            show_msg("%s: ATR file too short at sector %d", file_name, i + 1);
            break;
        }
    }
    fclose(f);
    // Ok, copy to image
    struct atr_image *atr = malloc(sizeof(struct atr_image));
    atr->data = data;
    atr->sec_size = ssz;
    atr->sec_count = num_sectors;
    return atr;
}