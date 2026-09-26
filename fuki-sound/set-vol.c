#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include "fuki.h"

void set_vol(int pcm_id, int hidari, int migi) {
    char dev_node[32];
    snprintf(dev_node, sizeof(dev_node), "/dev/mixer%d", pcm_id);

    int fd = open(dev_node, O_RDWR);
    if (fd < 0) return;

    int paket = (migi << 8) | hidari;
    ioctl(fd, MIXER_WRITE(SOUND_MIXER_VOLUME), &paket);
    close(fd);
}

void read_vol(AudioDevice *dev) {
    int fd = open(dev->dev_node, O_RDONLY);
    if (fd < 0) return;

    int paket = 0;
    if (ioctl(fd, MIXER_READ(SOUND_MIXER_VOLUME), &paket) >= 0) {
        dev->hidari = paket & 0x7f;
        dev->migi = (paket >> 8) & 0x7f;
    }
    close(fd);
}