#include <hpsys.h>
#include <hpgraphics.h>

int main(void)
{
    char data[] = { 0x55, 0xAA };

    hpg_pattern_t *pat = hpg_alloc_pattern(data, 2, 1);
    hpg_set_pattern(hpg_stdscreen, pat);

    // ...

    hpg_free_pattern(pat);

    return(0);
}
