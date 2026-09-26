#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include "fuki.h"

int read_ids(AudioDevice *dev_list, int max_devs) {
    FILE *fp = fopen("/dev/sndstat", "r");
    if (!fp) return 0;

    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), fp) && count < max_devs) {
        if (strncmp(line, "pcm", 3) == 0) {
            int pcm_id;
            char desc[128];

            if (sscanf(line, "pcm%d: <%127[^>]>", &pcm_id, desc) == 2) {
                dev_list[count].pcm_id = pcm_id;
                snprintf(dev_list[count].dev_node, sizeof(dev_list[count].dev_node), "/dev/mixer%d", pcm_id);
                strncpy(dev_list[count].name, desc, sizeof(dev_list[count].name) - 1);

                read_vol(&dev_list[count]);
                count++;
            }
        }
    }
    fclose(fp);
    return count;
}

int get_default_unit(void) {
    int default_unit = -1;
    size_t len = sizeof(default_unit);
    sysctlbyname("hw.snd.default_unit", &default_unit, &len, NULL, 0);
    return default_unit;
}

void set_default_unit(int pcm_id) {
    sysctlbyname("hw.snd.default_unit", NULL, NULL, &pcm_id, sizeof(pcm_id));
}

int main(void) {
    start_tui();
    return 0;
}