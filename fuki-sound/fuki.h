#ifndef fuki_h
#define fuki_h

typedef struct {
    int pcm_id;
    char name[128];
    char dev_node[32];
    int hidari;
    int migi;
} AudioDevice;

void set_vol(int pcm_id, int hidari, int migi);
void read_vol(AudioDevice *dev);

int read_ids(AudioDevice *dev_list, int max_devs);
int get_default_unit(void);
void set_default_unit(int pcm_id);

void start_tui(void);

#endif