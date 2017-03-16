/**
 * @Filename: aclk_dpi_fragment.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 12/29/2016 04:42:03 PM
 */
#include "aclk_sdk.h"
#include "aclk.h"

#include "aclk_dpi_fragment.h"

CVMX_SHARED aclk_hash_table_info_t *g_frag_table_v4;
CVMX_SHARED aclk_hash_table_info_t *g_frag_table_v6;
CVMX_SHARED cvmx_zone_t g_frag_table_zone_ext;
CVMX_SHARED cvmx_zone_t g_frag_item_zone_v4;
CVMX_SHARED cvmx_zone_t g_frag_item_zone_v6;


///SDBMHash
uint32_t aclk_dpi_frag_hash(uint8_t ver, ip_info_t sip, ip_info_t dip, uint32_t id)
{
    int i;
    uint32_t hash;

    hash = 0;
    if (4 == ver) {
        hash = (sip.ipv4 & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((sip.ipv4 >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((sip.ipv4 >> 2) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((sip.ipv4 >> 3) & 0xff) + (hash << 6) + (hash << 16) - hash;
        
        hash = (dip.ipv4 & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((dip.ipv4 >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((dip.ipv4 >> 2) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((dip.ipv4 >> 3) & 0xff) + (hash << 6) + (hash << 16) - hash;
        
        hash = (id & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((id >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((id >> 2) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((id >> 3) & 0xff) + (hash << 6) + (hash << 16) - hash;
    } else {
        for (i = 0; i < 8; i++) {
            hash = (sip.ipv6[i] & 0xff) + (hash << 6) + (hash << 16) - hash;
            hash = ((sip.ipv6[i] >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        }
        for (i = 0; i < 8; i++) {
            hash = (dip.ipv6[i] & 0xff) + (hash << 6) + (hash << 16) - hash;
            hash = ((dip.ipv6[i] >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        }
        hash = (id & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((id >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((id >> 2) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((id >> 3) & 0xff) + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

int frag_compare_cb_v4(void *item_a, void *item_b)
{
    frag_item_v4_t *frag_a, *frag_b;

    frag_a = (frag_item_v4_t *)item_a;
    frag_b = (frag_item_v4_t *)item_b;
    if (frag_a->sip != frag_b->sip) {
        return 1;
    }
    if (frag_a->dip != frag_b->dip) {
        return 1;
    }
    if (frag_a->id != frag_b->id) {
        return 1;
    }

    return 0;
}

int frag_compare_cb_v6(void *item_a, void *item_b)
{
    frag_item_v6_t *frag_a, *frag_b;

    frag_a = (frag_item_v6_t *)item_a;
    frag_b = (frag_item_v6_t *)item_b;
    if (0 != memcmp(frag_a->sip, frag_b->sip, 16)) {
        return 1;
    }
    if (0 != memcmp(frag_a->dip, frag_b->dip, 16)) {
        return 1;
    }
    if (frag_a->id != frag_b->id) {
        return 1;
    }

    return 0;
}

int aclk_dpi_fragment_init_local(void)
{
    return 0;
}

int aclk_dpi_fragment_init_global(void)
{
    void *ptr;

    ///malloc for frag table zone
    ptr = cvmx_bootmem_alloc_named(ACLK_FRAG_TABLE_SIZE * ACLK_FRAG_TABLE_ZONE_NUM_EXT, CVMX_CACHE_LINE_SIZE, ACLK_FRAG_TABLE_ZONE_EXT_NAME);
    if (ptr == NULL) {
         aclk_uart_printf("%s[%d]:aclk frag table zone ext malloc error\n", __func__, __LINE__);
         return -1;
    }
    g_frag_table_zone_ext = cvmx_zone_create_from_addr(ACLK_FRAG_TABLE_ZONE_EXT_NAME, ACLK_FRAG_TABLE_SIZE, ACLK_FRAG_TABLE_ZONE_NUM_EXT, ptr, ACLK_FRAG_TABLE_SIZE * ACLK_FRAG_TABLE_ZONE_NUM_EXT, 0);
    if (NULL == g_frag_table_zone_ext) {
        cvmx_bootmem_free_named(ACLK_FRAG_TABLE_ZONE_EXT_NAME);
        return -1;
    }

    ///malloc for frag talbe item v4
    ptr = cvmx_bootmem_alloc_named(ACLK_FRAG_ITEM_SIZE_V4 * ACLK_FRAG_ITEM_NUM_V4, CVMX_CACHE_LINE_SIZE, ACLK_FRAG_ITEM_ZONE_NAME_V4);
    if (ptr == NULL) {
         aclk_uart_printf("%s[%d]:aclk frag table item zone v4 malloc error\n", __func__, __LINE__);
         return -1;
    }
    g_frag_item_zone_v4 = cvmx_zone_create_from_addr(ACLK_FRAG_ITEM_ZONE_NAME_V4, ACLK_FRAG_ITEM_SIZE_V4, ACLK_FRAG_ITEM_NUM_V4, ptr, ACLK_FRAG_ITEM_SIZE_V4 * ACLK_FRAG_ITEM_NUM_V4, 0);
    if (NULL == g_frag_item_zone_v4) {
        cvmx_bootmem_free_named(ACLK_FRAG_ITEM_ZONE_NAME_V4);
        return -1;
    }

    ///malloc for frag talbe item v6
    ptr = cvmx_bootmem_alloc_named(ACLK_FRAG_ITEM_SIZE_V6 * ACLK_FRAG_ITEM_NUM_V6, CVMX_CACHE_LINE_SIZE, ACLK_FRAG_ITEM_ZONE_NAME_V6);
    if (ptr == NULL) {
         aclk_uart_printf("%s[%d]:aclk frag table item zone v6 malloc error\n", __func__, __LINE__);
         return -1;
    }
    g_frag_item_zone_v6 = cvmx_zone_create_from_addr(ACLK_FRAG_ITEM_ZONE_NAME_V6, ACLK_FRAG_ITEM_SIZE_V6, ACLK_FRAG_ITEM_NUM_V6, ptr, ACLK_FRAG_ITEM_SIZE_V6 * ACLK_FRAG_ITEM_NUM_V6, 0);
    if (NULL == g_frag_item_zone_v6) {
        cvmx_bootmem_free_named(ACLK_FRAG_ITEM_ZONE_NAME_V6);
        return -1;
    }

    g_frag_table_v4 = aclk_hash_table_create(ACLK_FRAG_TABLE_ZONE_NAME_v4, ACLK_FRAG_TABLE_SIZE, ACLK_FRAG_TABLE_NUM_V4, sizeof(frag_item_v4_t), g_frag_table_zone_ext, g_frag_item_zone_v4, frag_compare_cb_v4, ACLK_HASH_TABLE_LOCK_SPIN);
    if (NULL == g_frag_table_v4) {
        aclk_uart_printf("%s[%d]:create frag table v4 error\n", __func__, __LINE__);
        return -1;
    }
    g_frag_table_v6 = aclk_hash_table_create(ACLK_FRAG_TABLE_ZONE_NAME_v6, ACLK_FRAG_TABLE_SIZE, ACLK_FRAG_TABLE_NUM_V6, sizeof(frag_item_v6_t), g_frag_table_zone_ext, g_frag_item_zone_v6, frag_compare_cb_v6, ACLK_HASH_TABLE_LOCK_SPIN);
    if (NULL == g_frag_table_v6) {
        aclk_uart_printf("%s[%d]:create frag table v6 error\n", __func__, __LINE__);
        return -1;
    }

    return 0;
}

int aclk_dpi_fragment_packet_process_v4(cvmx_wqe_t *packet)
{
    int recode;
    uint32_t hash;
    frag_item_v4_t frag_item;
    aclk_dpi_pkt_info_t *pkt;

    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }

    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));
    if (0 == pkt->frag_offset) {
        ///first frag
        ///create frag hash table
        hash = aclk_dpi_frag_hash(pkt->ip_ver, pkt->sip, pkt->dip, pkt->id);
        hash = hash & ACLK_FRAG_TABLE_MASK_V4;
        frag_item.sip = pkt->sip.ipv4;
        frag_item.dip = pkt->dip.ipv4;
        frag_item.id = pkt->id;
        frag_item.sport = pkt->sport;
        frag_item.dport = pkt->dport;
        frag_item.l4_proto = pkt->l4_proto;
        recode = aclk_hash_table_find(g_frag_table_v4, hash, &frag_item);
        if (recode) {
            ///not find
            recode = aclk_hash_table_add(g_frag_table_v4, hash, &frag_item);
            if (!recode) {
                aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: add frag item to table error\n", __func__, __LINE__);
                return -1;
            }
        }
        /// else do nothing
    } else {
        ///not first frag, look up frag table; 
        hash = aclk_dpi_frag_hash(pkt->ip_ver, pkt->sip, pkt->dip, pkt->id);
        hash = hash & ACLK_FRAG_TABLE_MASK_V4;
        frag_item.sip = pkt->sip.ipv4;
        frag_item.dip = pkt->dip.ipv4;
        frag_item.id = pkt->id;
        recode = aclk_hash_table_find(g_frag_table_v4, hash, &frag_item);
        if (!recode) {
            pkt->sport = frag_item.sport;
            pkt->dport = frag_item.dport;
            pkt->l4_proto = frag_item.l4_proto;

            if (!(pkt->frag_more)) {
                aclk_hash_table_del(g_frag_table_v4, hash, &frag_item);
            }
        }
    }


    return 0;
}

int aclk_dpi_fragment_packet_process_v6(cvmx_wqe_t *packet)
{
    int recode;
    uint32_t hash;
    frag_item_v6_t frag_item;
    aclk_dpi_pkt_info_t *pkt;

    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }

    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));
    if (0 == pkt->frag_offset) {
        ///firt frag
        ///create frag hash table
        hash = aclk_dpi_frag_hash(pkt->ip_ver, pkt->sip, pkt->dip, pkt->id);
        hash = hash & ACLK_FRAG_TABLE_MASK_V6;
        memcpy(frag_item.sip, pkt->sip.ipv6, 16);
        memcpy(frag_item.dip, pkt->dip.ipv6, 16);
        frag_item.id = pkt->id;
        frag_item.sport = pkt->sport;
        frag_item.dport = pkt->dport;
        frag_item.l4_proto = pkt->l4_proto;
        recode = aclk_hash_table_find(g_frag_table_v4, hash, &frag_item);
        if (recode) {
            ///not find
            recode = aclk_hash_table_add(g_frag_table_v4, hash, &frag_item);
            if (!recode) {
                aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: add frag item to table error\n", __func__, __LINE__);
                return -1;
            }
        }
    } else {
        ///lookup frag hash table
        ///not first frag, look up frag table; 
        hash = aclk_dpi_frag_hash(pkt->ip_ver, pkt->sip, pkt->dip, pkt->id);
        hash = hash & ACLK_FRAG_TABLE_MASK_V6;
        memcpy(frag_item.sip, pkt->sip.ipv6, 16);
        memcpy(frag_item.dip, pkt->dip.ipv6, 16);
        frag_item.id = pkt->id;
        recode = aclk_hash_table_find(g_frag_table_v4, hash, &frag_item);
        if (!recode) {
            pkt->sport = frag_item.sport;
            pkt->dport = frag_item.dport;
            pkt->l4_proto = frag_item.l4_proto;

            if (!(pkt->frag_more)) {
                aclk_hash_table_del(g_frag_table_v4, hash, &frag_item);
            }
        }
    }

    return 0;
}

int aclk_dpi_fragment_packet_process(cvmx_wqe_t *packet)
{
    aclk_dpi_pkt_info_t *pkt;

    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }

    pkt = (aclk_dpi_pkt_info_t *)((uint8_t *)(packet->packet_data));
    if (4 == pkt->ip_ver) {
        if (aclk_dpi_fragment_packet_process_v4(packet)) {
            return -1;
        }
    } else if (6 == pkt->ip_ver) {
        if (aclk_dpi_fragment_packet_process_v6(packet)) {
            return -1;
        }
    } else {
        pkt->appidx = ACLK_PIDE_PROTO_FRAG;
        return -1;
    }

    return 0;
}

void aclk_dpi_fragment_fini_local(void)
{
    return;
}

void aclk_dpi_fragment_fini_global(void)
{
    return;
}
