#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "fuki.h"

static struct termios orig_termios;

static void disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\033[?25h\033[?1049l");
}

static void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    printf("\033[?1049h\033[?25l");
}

static void print_bar(int val) {
    int filled = val / 10;
    printf("[");
    for (int i = 0; i < 10; i++) {
        if (i < filled) printf("▓");
        else printf("░");
    }
    printf("] %3d%%", val);
}

void start_tui(void) {
    enable_raw_mode();

    AudioDevice dev_list[16];
    int selected = 0;
    char c;

    while (1) {
        int count = read_ids(dev_list, 16);
        if (selected >= count && count > 0) selected = count - 1;
        if (selected < 0) selected = 0;

        int active_unit = get_default_unit();

        printf("\033[2J\033[H");

        printf("╔═══════════════════════════════════════════════════════════════════════════╗\n");
        printf("║                         FUKI-SOUND AUDIO MIXER                            ║\n");
        printf("╠═══════════════════════════════════════════════════════════════════════════╣\n");

        if (count == 0) {
            printf("║  Cihaz bulunamadi...                                                     ║\n");
        } else {
            for (int i = 0; i < count; i++) {
                int is_sel = (i == selected);
                int is_active = (dev_list[i].pcm_id == active_unit);

                printf("║ ");
                if (is_sel) {
                    printf("\033[1;7m>");
                } else {
                    printf(" ");
                }

                printf(" %c pcm%-2d %-25.25s L:", 
                        is_active ? '*' : ' ', 
                        dev_list[i].pcm_id, 
                        dev_list[i].name);

                print_bar(dev_list[i].hidari);
                printf(" R:");
                print_bar(dev_list[i].migi);

                if (is_sel) {
                    printf("\033[0m");
                }
                printf(" ║\n");
            }
        }

        printf("╠═══════════════════════════════════════════════════════════════════════════╣\n");
        printf("║ \033[1;33m[↑/↓]\033[0m: Sec │ \033[1;33m[←/→]\033[0m: Ses │ \033[1;33m[Space]\033[0m: Aktif Yap │ \033[1;33m[q]\033[0m: Cikis                 ║\n");
        printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");

        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == 'q') break;

            if (c == ' ' && count > 0) {
                set_default_unit(dev_list[selected].pcm_id);
            }

            if (c == '\033') {
                char seq[2];
                if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                    if (seq[0] == '[') {
                        switch (seq[1]) {
                            case 'A':
                                if (selected > 0) selected--;
                                break;
                            case 'B':
                                if (selected < count - 1) selected++;
                                break;
                            case 'C':
                                if (count > 0) {
                                    int cur_h = dev_list[selected].hidari;
                                    int cur_m = dev_list[selected].migi;
                                    set_vol(dev_list[selected].pcm_id, 
                                            cur_h + 5 > 100 ? 100 : cur_h + 5, 
                                            cur_m + 5 > 100 ? 100 : cur_m + 5);
                                }
                                break;
                            case 'D':
                                if (count > 0) {
                                    int cur_h = dev_list[selected].hidari;
                                    int cur_m = dev_list[selected].migi;
                                    set_vol(dev_list[selected].pcm_id, 
                                            cur_h - 5 < 0 ? 0 : cur_h - 5, 
                                            cur_m - 5 < 0 ? 0 : cur_m - 5);
                                }
                                break;
                        }
                    }
                }
            }
        }
    }
}