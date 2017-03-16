/*
 * @Filename: aclk_dpi_fragment.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:36:00 PM
 */
#ifndef __ACLK_DPI_FRAGMENT_H__
#define __ACLK_DPI_FRAGMENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FRAG_TABLE_WIDTH            14

#define ACLK_FRAG_TABLE_ZONE_NAME_v4    "frag_table_zone_v4"
#define ACLK_FRAG_TABLE_ZONE_NAME_v6    "frag_table_zone_v6"
#define ACLK_FRAG_TABLE_ZONE_EXT_NAME   "frag_table_zone_ext"
#define ACLK_FRAG_ITEM_ZONE_NAME_V4     "frag_item_zone_v4"
#define ACLK_FRAG_ITEM_ZONE_NAME_V6     "frag_item_zone_v6"

#define ACLK_FRAG_TABLE_NUM_V4      (1lu << FRAG_TABLE_WIDTH)
#define ACLK_FRAG_TABLE_NUM_V6      (1lu << FRAG_TABLE_WIDTH)
#define ACLK_FRAG_TABLE_MASK_V4     (ACLK_FRAG_TABLE_NUM_V4 - 1)
#define ACLK_FRAG_TABLE_MASK_V6     (ACLK_FRAG_TABLE_NUM_V6 - 1)
#define ACLK_FRAG_TABLE_SIZE        64

///
#define ACLK_FRAG_TABLE_ZONE_NUM_EXT    0x3fff

#define ACLK_FRAG_ITEM_NUM_V4       (ACLK_FRAG_TABLE_NUM_V4 * 4)
#define ACLK_FRAG_ITEM_NUM_V6       (ACLK_FRAG_TABLE_NUM_V6 * 4)
#define ACLK_FRAG_ITEM_SIZE_V4  128
#define ACLK_FRAG_ITEM_SIZE_V6  256

typedef struct frag_item_v4 {
    uint32_t sip;
    uint32_t dip;
    uint16_t sport;
    uint16_t dport;
    uint16_t id;
    uint8_t  l4_proto;
} frag_item_v4_t;

typedef struct frag_item_v6 {
    uint16_t sip[8];
    uint16_t dip[8];
    uint16_t sport;
    uint16_t dport;
    uint32_t id;
    uint8_t  l4_proto;
} frag_item_v6_t;

typedef struct ipv6_frag {
    uint8_t next_proto;
    uint8_t res1;
    uint16_t offset;
    uint32_t id;
} ipv6_frag_t;

int aclk_dpi_fragment_init_local(void);
int aclk_dpi_fragment_init_global(void);

int aclk_dpi_fragment_packet_process(cvmx_wqe_t *packet);

void aclk_dpi_fragment_fini_local(void);
void aclk_dpi_fragment_fini_global(void);

#ifdef __cplusplus
}
#endif
#endif
