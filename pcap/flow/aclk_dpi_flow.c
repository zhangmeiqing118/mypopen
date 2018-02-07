/**
 * @File Name: aclk_dpi_connect.c
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Tue 03 May 2016 02:28:14 PM CST
 */
#include "aclk_sdk.h"
#include "aclk.h"
#include "pide.h"

///flow table and zone ptr
CVMX_SHARED aclk_connect_hash_table_info_t *g_connect_table_v4;
CVMX_SHARED aclk_connect_hash_table_info_t *g_connect_table_v6;
CVMX_SHARED cvmx_zone_t g_connect_zone_v4;
CVMX_SHARED cvmx_zone_t g_connect_zone_v6;
///
static int g_local_core_id;
CVMX_SHARED aclk_dpi_connect_stat_t g_connect_stat[MAX_CORES];
CVMX_SHARED uint64_t g_connect_cycle_timeout;

#if 0
///connect command define
static void aclk_connect_timeout_update_v4(aclk_dpi_ipv4_flow_info_t *flow)
{
    aclk_pide_flow_timeout_stat(flow->appidx, flow->seq, flow->bytes);

    return;
}

#ifdef CONNECT_IPV6
static void aclk_connect_timeout_update_v6(aclk_dpi_ipv6_flow_info_t *flow)
{
    aclk_pide_flow_timeout_stat(flow->appidx, flow->seq, flow->bytes);

    return;
}
#endif

void aclk_connect_clear_match_all_result(void)
{
    uint32_t i;
    uint8_t idx;
    uint32_t table_num;
    aclk_connect_hash_table_item_header_t *hd;
    aclk_connect_hash_table_item_t *item_hd, *item;
    aclk_dpi_ipv4_flow_info_t *flow_v4;
#ifdef CONNECT_IPV6
    aclk_dpi_ipv6_flow_info_t *flow_v6;
#endif

#ifdef CONNECT_IPV6
    table_num = (g_connect_table_v4->table_num > g_connect_table_v6->table_num) ? g_connect_table_v4->table_num : g_connect_table_v6->table_num;
#else
    table_num = g_connect_table_v4->table_num;
#endif
    for (i = 0; i < table_num; i++) {
        cvmx_pow_tag_sw_full((cvmx_wqe_t *)0x80, i, CVMX_POW_TAG_TYPE_ATOMIC, 0);
        cvmx_pow_tag_sw_wait();
        if (i < g_connect_table_v4->table_num) {
            idx = 0;
            hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v4->table + g_connect_table_v4->table_size * i);
            item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
            for (idx = 0; idx < hd->item_used; idx++) {
                item = item_hd + idx;
                flow_v4 = (aclk_dpi_ipv4_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
                flow_v4->appidx = 0;
                flow_v4 ->seq = 0;
                flow_v4->bytes = 0;
            }
        }

#ifdef CONNECT_IPV6
        if (i < g_connect_table_v6->table_num) {
            idx = 0;
            hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v6->table + g_connect_table_v6->table_size * i);
            item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
            for (idx = 0; idx < hd->item_used; idx++) {
                item = item_hd + idx;
                flow_v6 = (aclk_dpi_ipv6_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
                flow_v6->appidx = 0;
                flow_v6 ->seq = 0;
                flow_v6->bytes = 0;
            }
        }
#endif
        cvmx_pow_tag_sw_null();
    }

    return;
}

void aclk_connect_clear_match_result(uint32_t appidx)
{
    uint32_t i;
    uint8_t idx;
    uint32_t table_num;
    aclk_connect_hash_table_item_header_t *hd;
    aclk_connect_hash_table_item_t *item_hd, *item;
    aclk_dpi_ipv4_flow_info_t *flow_v4;
#ifdef CONNECT_IPV6
    aclk_dpi_ipv6_flow_info_t *flow_v6;
#endif

#ifdef CONNECT_IPV6
    table_num = (g_connect_table_v4->table_num > g_connect_table_v6->table_num) ? g_connect_table_v4->table_num : g_connect_table_v6->table_num;
#else
    table_num = g_connect_table_v4->table_num;
#endif
    for (i = 0; i < table_num; i++) {
        cvmx_pow_tag_sw_full((cvmx_wqe_t *)0x80, i, CVMX_POW_TAG_TYPE_ATOMIC, 0);
        cvmx_pow_tag_sw_wait();
        if (i < g_connect_table_v4->table_num) {
            idx = 0;
            hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v4->table + g_connect_table_v4->table_size * i);
            item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
            for (idx = 0; idx < hd->item_used; idx++) {
                item = item_hd + idx;
                flow_v4 = (aclk_dpi_ipv4_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
                if (appidx == flow_v4->appidx) {
                    flow_v4->appidx = 0;
                } else if (appidx < flow_v4->appidx) {
                    flow_v4->appidx -= 1;
                }
            }
        }

#ifdef CONNECT_IPV6
        if (i < g_connect_table_v6->table_num) {
            idx = 0;
            hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v6->table + g_connect_table_v6->table_size * i);
            item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
            for (idx = 0; idx < hd->item_used; idx++) {
                item = item_hd + idx;
                flow_v6 = (aclk_dpi_ipv6_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
                if (appidx == flow_v6->appidx) {
                    flow_v6->appidx = 0;
                } else if (appidx < flow_v6->appidx) {
                    flow_v6->appidx -= 1;
                }
            }
        }
#endif
        cvmx_pow_tag_sw_null();
    }

    return;
}

void aclk_connect_get_match_result(void *array)
{
    uint32_t i;
    uint8_t idx;
    uint32_t table_num;
    aclk_pide_result_stat_t *stat;
    aclk_connect_hash_table_item_header_t *hd;
    aclk_connect_hash_table_item_t *item_hd, *item;
    aclk_dpi_ipv4_flow_info_t *flow_v4;
#ifdef CONNECT_IPV6
    aclk_dpi_ipv6_flow_info_t *flow_v6;
#endif

    stat = (aclk_pide_result_stat_t *)array;
#ifdef CONNECT_IPV6
    table_num = (g_connect_table_v4->table_num > g_connect_table_v6->table_num) ? g_connect_table_v4->table_num : g_connect_table_v6->table_num;
#else
    table_num = g_connect_table_v4->table_num;
#endif
    for (i = 0; i < table_num; i++) {
        cvmx_pow_tag_sw_full((cvmx_wqe_t *)0x80, i, CVMX_POW_TAG_TYPE_ATOMIC, 0);
        cvmx_pow_tag_sw_wait();
        if (i < g_connect_table_v4->table_num) {
            idx = 0;
            hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v4->table + g_connect_table_v4->table_size * i);
            item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
            for (idx = 0; idx < hd->item_used; idx++) {
                item = item_hd + idx;
                flow_v4 = (aclk_dpi_ipv4_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
                stat[flow_v4->appidx].packet[g_local_core_id] += flow_v4->seq;
                stat[flow_v4->appidx].bytes[g_local_core_id] += flow_v4->bytes;
            }
        }

#ifdef CONNECT_IPV6
        if (i < g_connect_table_v6->table_num) {
            idx = 0;
            hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v6->table + g_connect_table_v6->table_size * i);
            item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
            for (idx = 0; idx < hd->item_used; idx++) {
                item = item_hd + idx;
                flow_v6 = (aclk_dpi_ipv6_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
                stat[flow_v6->appidx].packet[g_local_core_id] += flow_v6->seq;
                stat[flow_v6->appidx].bytes[g_local_core_id] += flow_v6->bytes;
            }
        }
#endif
        cvmx_pow_tag_sw_null();
    }

    return;
}

void aclk_connect_cmd_show_timeout(int argc, char *argv[])
{
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("connect table timeout %ld\n", g_connect_cycle_timeout / ((uint64_t)(cvmx_sysinfo_get()->cpu_clock_hz)));
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "connect table timeout: %ld\r\n", g_connect_cycle_timeout / ((uint64_t)(cvmx_sysinfo_get()->cpu_clock_hz)));
    }
    return;
}

void aclk_connect_cmd_show_stat(int argc, char *argv[])
{
    int i;
    uint64_t counter;

    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].tcp_add_stream_num;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("tcp add stream num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "tcp add stream num:   %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].tcp_del_stream_num;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("tcp del stream num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "tcp del stream num:   %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].tcp_add_packet_num;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("tcp add packet num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "tcp add packet num:   %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].tcp_add_bytes;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("tcp add bytes: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "tcp add bytes:        %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].udp_add_stream_num;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("udp add stream num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "udp add stream num:   %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].udp_del_stream_num;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("udp del stream num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "udp del stream num:   %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].udp_add_packet_num;
    }    
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("udp add packet num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "udp add packet num:   %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].udp_add_bytes;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("udp add bytes: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "udp add bytes:        %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].total_add_stream_num;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("total add stream num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "total add stream num: %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].total_del_stream_num;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("total del stream num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "total del stream num: %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].total_add_packet_num;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("total add packet num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "total add packet num: %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].total_add_bytes;
    }    
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("total add bytes: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "total add bytes:      %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].drop_packet_num;
    }    
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("drop packet num: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "drop packet num:      %lu\r\n", counter);
    }
    counter = 0;
    for (i = 0; i < MAX_CORES; i++) {
        counter += g_connect_stat[i].drop_bytes;
    }   
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("drop bytes: %lu\n", counter);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "drop bytes:           %lu\r\n", counter);
    }
    return;
}

void aclk_connect_cmd_show_table(int argc, char *argv[])
{
    uint32_t i;
    uint32_t max_depth, zone_used;
    aclk_connect_hash_table_item_header_t *hd;
    
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("==================ipv4 hash table info:==================\n");
        aclk_uart_printf("table         num:%u\n", g_connect_table_v4->table_num);
        aclk_uart_printf("table bucket size:%u\n", g_connect_table_v4->table_size);
        aclk_uart_printf("table    item num:%u\n", g_connect_table_v4->item_num);
        aclk_uart_printf("table   item size:%u\n", g_connect_table_v4->item_size);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "======ipv4 hash table info:======\r\n");        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table num:         %u\r\n", g_connect_table_v4->table_num);        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table bucket size: %u\r\n", g_connect_table_v4->table_size);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table item num:    %u\r\n", g_connect_table_v4->item_num);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table item size:   %u\r\n", g_connect_table_v4->item_size);
    }
    max_depth = 0;
    zone_used = 0;
    for (i = 0; i < g_connect_table_v4->table_num; i++) {
        hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v4->table + g_connect_table_v4->table_size * i);
        if (max_depth < hd->item_used) {
            max_depth = hd->item_used;
        }
        zone_used += hd->item_used;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("table  max  depth:%u\n", max_depth);
        aclk_uart_printf("zone   total  num:%u\n", ACLK_CONNECT_ZONE_V4_NUM);
        aclk_uart_printf("zone    used  num:%u\n", zone_used);
    } else {              
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table max depth:   %u\r\n", max_depth);        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "zone total num:    %lu\r\n", ACLK_CONNECT_ZONE_V4_NUM);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "zone used num:     %u\r\n", zone_used);
    }
   
#ifdef CONNECT_IPV6  
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("==================ipv6 hash table info:==================\n");
        aclk_uart_printf("table num:         %u\n", g_connect_table_v6->table_num);
        aclk_uart_printf("table bucket size: %u\n", g_connect_table_v6->table_size);
        aclk_uart_printf("table item num:    %u\n", g_connect_table_v6->item_num);
        aclk_uart_printf("table item size:   %u\n", g_connect_table_v6->item_size);
    }  else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "======ipv6 hash table info:======\r\n");        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table num:         %u\r\n", g_connect_table_v6->table_num);        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table bucket size: %u\r\n", g_connect_table_v6->table_size);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table item num:    %u\r\n", g_connect_table_v6->item_num);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table item size:   %u\r\n", g_connect_table_v6->item_size);
    }
    max_depth = 0;
    zone_used = 0;
    for (i = 0; i < g_connect_table_v6->table_num; i++) {
        hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v6->table + g_connect_table_v6->table_size * i);
        if (max_depth < hd->item_used) {
            max_depth = hd->item_used;
        }
        zone_used += hd->item_used;
    }   
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("table  max  depth:%u\n", max_depth);
        aclk_uart_printf("zone   total  num:%u\n", ACLK_CONNECT_ZONE_V6_NUM);
        aclk_uart_printf("zone    used  num:%u\n", zone_used);
    } else {              
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table max depth:   %u\r\n", max_depth);        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "zone total num:    %lu\r\n", ACLK_CONNECT_ZONE_V6_NUM);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "zone used num:     %u\r\n", zone_used);
    }
#endif

    return;
}
void aclk_connect_cmd_show_info(int argc, char *argv[])
{
    uint32_t i;
    uint32_t max_depth, zone_used;
    aclk_connect_hash_table_item_header_t *hd;
    
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("==================ipv4 hash table info:==================\n");
        aclk_uart_printf("table         num:%u\n", g_connect_table_v4->table_num);
        aclk_uart_printf("table bucket size:%u\n", g_connect_table_v4->table_size);
        aclk_uart_printf("table    item num:%u\n", g_connect_table_v4->item_num);
        aclk_uart_printf("table   item size:%u\n", g_connect_table_v4->item_size);
    } else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "======ipv4 hash table info:======\r\n");        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table num:         %u\r\n", g_connect_table_v4->table_num);        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table bucket size: %u\r\n", g_connect_table_v4->table_size);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table item num:    %u\r\n", g_connect_table_v4->item_num);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table item size:   %u\r\n", g_connect_table_v4->item_size);
    }
    max_depth = 0;
    zone_used = 0;
    for (i = 0; i < g_connect_table_v4->table_num; i++) {
        hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v4->table + g_connect_table_v4->table_size * i);
        if (max_depth < hd->item_used) {
            max_depth = hd->item_used;
        }
        zone_used += hd->item_used;
    }
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("table  max  depth:%u\n", max_depth);
        aclk_uart_printf("zone   total  num:%u\n", ACLK_CONNECT_ZONE_V4_NUM);
        aclk_uart_printf("zone    used  num:%u\n", zone_used);
    } else {              
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table max depth:   %u\r\n", max_depth);        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "zone total num:    %lu\r\n", ACLK_CONNECT_ZONE_V4_NUM);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "zone used num:     %u\r\n", zone_used);
    }
   
#ifdef CONNECT_IPV6  
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("==================ipv6 hash table info:==================\n");
        aclk_uart_printf("table num:         %u\n", g_connect_table_v6->table_num);
        aclk_uart_printf("table bucket size: %u\n", g_connect_table_v6->table_size);
        aclk_uart_printf("table item num:    %u\n", g_connect_table_v6->item_num);
        aclk_uart_printf("table item size:   %u\n", g_connect_table_v6->item_size);
    }  else {
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "======ipv6 hash table info:======\r\n");        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table num:         %u\r\n", g_connect_table_v6->table_num);        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table bucket size: %u\r\n", g_connect_table_v6->table_size);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table item num:    %u\r\n", g_connect_table_v6->item_num);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table item size:   %u\r\n", g_connect_table_v6->item_size);
    }
    max_depth = 0;
    zone_used = 0;
    for (i = 0; i < g_connect_table_v6->table_num; i++) {
        hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v6->table + g_connect_table_v6->table_size * i);
        if (max_depth < hd->item_used) {
            max_depth = hd->item_used;
        }
        zone_used += hd->item_used;
    }   
    if (!g_shell_cmd_flag) {
        aclk_uart_printf("table  max  depth:%u\n", max_depth);
        aclk_uart_printf("zone   total  num:%u\n", ACLK_CONNECT_ZONE_V6_NUM);
        aclk_uart_printf("zone    used  num:%u\n", zone_used);
    } else {              
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "table max depth:   %u\r\n", max_depth);        
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "zone total num:    %lu\r\n", ACLK_CONNECT_ZONE_V6_NUM);
        g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "zone used num:     %u\r\n", zone_used);
    }
#endif

    return;
}

void aclk_connect_cmd_show(int argc, char *argv[])
{
    if (argc == 1) {
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("Usage:\n");
            aclk_uart_printf("  connect show timeout\n");
            aclk_uart_printf("  connect show stat\n");
            aclk_uart_printf("  connect show table\n");
            aclk_uart_printf("  connect show info\n");
        } else {       
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\r\n");
        }
        return;
    }
    if (0 == strcmp(argv[1], "timeout")) {
        aclk_connect_cmd_show_timeout(argc - 1, &(argv[1]));
    } else if (0 == strcmp(argv[1], "stat")) {
        aclk_connect_cmd_show_stat(argc - 1, &(argv[1]));
    } else if (0 == strcmp(argv[1], "table")) {
        aclk_connect_cmd_show_table(argc - 1, &(argv[1]));
    } else if (0 == strcmp(argv[1], "info")) {
        aclk_connect_cmd_show_info(argc - 1, &(argv[1]));
    } else {        
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("%s[%d]:Invalid connet show command\n", __func__, __LINE__);
        } else {
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\r\n");
        }
    }

    return;
}

void aclk_connect_cmd_set_timeout(int argc, char *argv[])
{
    int timeout;

    if (argc == 1) {        
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("Usage:\n");
            aclk_uart_printf("  connect set timeout [timeout:5-120 sec, default: 30]\n");
        } else {
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\n");
        }
        return;
    }
    timeout = atoi(argv[1]);
    if (timeout >= MIN_CONNECT_TIMEOUT && timeout <= MAX_CONNECT_TIMEOUT) {
        g_connect_cycle_timeout = timeout * ((uint64_t)(cvmx_sysinfo_get()->cpu_clock_hz));
    } else {       
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("Invalid timeout(%d), valid range(%d-%d)\n", timeout, MIN_CONNECT_TIMEOUT, MAX_CONNECT_TIMEOUT);
        } else {
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\n");
        }
    }

    return;
}

void aclk_connect_cmd_set(int argc, char *argv[])
{
    if (argc == 1) {
        aclk_uart_printf("Usage:\n");
        aclk_uart_printf("  connect set timeout [timeout]\n");

        return;
    }
    if (0 == strcmp(argv[1], "timeout")) {
        aclk_connect_cmd_set_timeout(argc - 1, &(argv[1]));
    } else {        
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("%s[%d]:Invalid connet set command\n", __func__, __LINE__);
        } else {
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\n");
        }
    }

    return;
}

void aclk_connect_cmd(int argc, char *argv[])
{
    if (argc == 1) {        
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("Usage:\n");
            aclk_uart_printf("  connect show timeout\n");
            aclk_uart_printf("  connect show stat\n");
            aclk_uart_printf("  connect show table\n");
            aclk_uart_printf("  connect set timeout [timeout] \n");
        } else {
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\n");
        }
        return;
    }
    if (0 == strcmp(argv[1], "show")) {
        aclk_connect_cmd_show(argc - 1, &(argv[1]));
    } else if (0 == strcmp(argv[1], "set")) {
        aclk_connect_cmd_set(argc - 1, &(argv[1]));
    } else {
        if (!g_shell_cmd_flag) {
            aclk_uart_printf("%s[%d]:Invalid connet command\n", __func__, __LINE__);
        } else {
            g_command_response_len += sprintf(g_command_response_buffer + g_command_response_len, "Invalid command\n");
        }
    }

    return;
}

///end connect command define

static uint32_t get_five_tuple_v4_hash(uint32_t sip, uint32_t dip, uint16_t sport, uint16_t dport, uint8_t protocol) 
{
    ///crc hash
    uint32_t crc;

    CVMX_MT_CRC_POLYNOMIAL(((uint64_t)POLY) << (32 - BIT_WIDTH_V4));
    CVMX_MT_CRC_IV(((uint64_t)0xffffffff) << (32 - BIT_WIDTH_V4));
    if (sip <= dip) {
        CVMX_MT_CRC_WORD(sip);
        CVMX_MT_CRC_WORD(dip);
        CVMX_MT_CRC_HALF(sport);
        CVMX_MT_CRC_HALF(dport);
    } else {
        CVMX_MT_CRC_WORD(dip);
        CVMX_MT_CRC_WORD(sip);
        CVMX_MT_CRC_HALF(dport);
        CVMX_MT_CRC_HALF(sport);
    }
    CVMX_MT_CRC_BYTE(protocol);
    CVMX_MF_CRC_IV(crc);

    return (crc >> (32 - BIT_WIDTH_V4));
}

//static 
uint32_t get_five_tuple_v6_hash(uint16_t *sip, uint16_t *dip, uint16_t sport, uint16_t dport, uint8_t protocol) 
{
    ///crc hash
    uint32_t i, crc;

    CVMX_MT_CRC_POLYNOMIAL(((uint64_t)POLY) << (32 - BIT_WIDTH_V6));
    CVMX_MT_CRC_IV(((uint64_t)0xffffffff) << (32 - BIT_WIDTH_V6));
    if ( *((uint64_t *)sip) <= *((uint64_t *)dip)) {
        for (i = 0; i < 8; i++) {
            CVMX_MT_CRC_WORD(sip[i]);
        }
        for (i = 0; i < 8; i++) {
            CVMX_MT_CRC_WORD(dip[i]);
        }
        CVMX_MT_CRC_HALF(sport);
        CVMX_MT_CRC_HALF(dport);
    } else {
        for (i = 0; i < 8; i++) {
            CVMX_MT_CRC_WORD(dip[i]);
        }
        for (i = 0; i < 8; i++) {
            CVMX_MT_CRC_WORD(sip[i]);
        }
        CVMX_MT_CRC_HALF(dport);
        CVMX_MT_CRC_HALF(sport);
    }
    CVMX_MT_CRC_BYTE(protocol);
    CVMX_MF_CRC_IV(crc);

    return (crc >> (32 - BIT_WIDTH_V6));
}
#endif

///SDBMHash
uint32_t aclk_dpi_hash(uint8_t ver, ip_info_t sip, ip_info_t dip, uint16_t sport, uint16_t dport, uint8_t protocol) 
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
    
        hash = (sport & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((sport >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = (dport & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((dport >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = (protocol & 0xff) + (hash << 6) + (hash << 16) - hash;
    } else {
        for (i = 0; i < 8; i++) {
            hash = (sip.ipv6[i] & 0xff) + (hash << 6) + (hash << 16) - hash;
            hash = ((sip.ipv6[i] >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        }   
        for (i = 0; i < 8; i++) {
            hash = (dip.ipv6[i] & 0xff) + (hash << 6) + (hash << 16) - hash;
            hash = ((dip.ipv6[i] >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        }
        hash = (sport & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((sport >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = (dport & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = ((dport >> 1) & 0xff) + (hash << 6) + (hash << 16) - hash;
        hash = (protocol & 0xff) + (hash << 6) + (hash << 16) - hash;
    }   

    return hash;
}

///tcp: judge by TCP three-way handshake
//others:first packet is up flow packet
int session_flow_dir(void *packet)
{
    uint8_t flag;
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;
    tcp_header_t *tcp;

    ///get packet info
    wqe = (cvmx_wqe_t *)packet;
    pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    if (6 == pkt->l4_proto) {
        tcp = (tcp_header_t *)(wqe->packet_ptr.s.addr + pkt->payload_offset - sizeof(tcp_header_t));
        flag = tcp->syn | tcp->ack;
        if ((0x12 == flag) || ((1 != tcp->ackno) && (0x10 == flag))) {
            return DOWN_FLOW;
        } 
        return UP_FLOW;
    }

    return UP_FLOW;
}

///tcp: judge by TCP four wave
///others:first packet is up flow packet
///when receive fin packet, flow packet left is 3
int session_flow_over(void *packet)
{
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;
    tcp_header_t *tcp;

    ///get packet info
    wqe = (cvmx_wqe_t *)packet;
    pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    if (6 == pkt->l4_proto) {
        tcp = (tcp_header_t *)(wqe->packet_ptr.s.addr + pkt->payload_offset - sizeof(tcp_header_t));
        if (tcp->fin) {
            return 1;
        } 
    }

    return 0;
}

/**
 * @brief session_table_compare_cb_v4 
 *
 * @param item_a
 * @param item_b
 *
 * @return if item_a == item_b, then return 0, else return 1.
 */
int session_table_compare_cb_v4(void *item_a, void *item_b)
{
    flow_five_tuple_v4_t *src, *dst;

    src = (flow_five_tuple_v4_t *)item_a;
    dst = (flow_five_tuple_v4_t *)item_b;

    ///printf("%s[%u]: itme_a,sip:%x, dip:%x, sport:%u, dport:%u, proto:%u\n", __func__, __LINE__, src->sip, src->dip, src->sport, src->dport, src->l4_proto);
    ///printf("%s[%u]: itme_b,sip:%x, dip:%x, sport:%u, dport:%u, proto:%u\n", __func__, __LINE__, dst->sip, dst->dip, dst->sport, dst->dport, dst->l4_proto);
    if (src->l4_proto != dst->l4_proto) {
        return 1;
    }
    if (((src->sip != dst->sip) || (src->dip != dst->dip))
                && ((src->sip != dst->dip) || (src->dip != dst->sip))) {
        return 1;
    }
    if (((src->sport != dst->sport) || (src->dport != dst->dport))
            && ((src->sport != dst->dport) || (src->dport != dst->sport))) {
        return 1;
    }

    return 0;
}

int session_table_compare_cb_v6(void *item_a, void *item_b)
{
    int i;
    flow_five_tuple_v6_t *src, *dst;

    src = (flow_five_tuple_v6_t *)item_a;
    dst = (flow_five_tuple_v6_t *)item_b;
    if (src->l4_proto != dst->l4_proto) {
        return 1;
    }
    for (i = 0; i < 8; i++) {
        if (((src->sip[i] != dst->sip[i]) || (src->dip[i] != dst->dip[i]))
                && ((src->sip[i] != dst->sip[i]) || (src->dip[i] != dst->dip[i]))) {
            return 1;
        }
    }
    if (((src->sport != dst->sport) || (src->dport != dst->dport))
            && ((src->sport != dst->dport) || (src->dport != dst->sport))) {
        return 1;
    }

    return 0;
}

#if 0
void aclk_connect_timer_callback_v4(aclk_connect_hash_table_info_t *table, uint32_t hash)
{
    uint8_t idx;
    uint64_t cycles;
    aclk_dpi_ipv4_flow_info_t *flow;
    aclk_connect_hash_table_item_header_t *hd;
    aclk_connect_hash_table_item_t *item_hd, *item;

    if (NULL == table) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: table is null\n", __func__, __LINE__);
        return;
    }
    if (hash >= table->table_num) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:Invalid hash value %u(max:%u)\n", __func__, __LINE__, hash, table->table_num);
        return;
    }

    idx = 0;
    cycles = cvmx_get_cycle();
    hd = (aclk_connect_hash_table_item_header_t *)(table->table + table->table_size * hash);
    item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
    for (idx = 0; idx < hd->item_used; idx++) {
        item = item_hd + idx;
        flow = (aclk_dpi_ipv4_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
        //printf("cycles:%lu, cycles:%lu, timeout:%lu\n", cycles, flow->last_time, g_connect_cycle_timeout);
        if (cycles - flow->last_time > g_connect_cycle_timeout) {
            ///flow timeout
            if (flow->l4_proto == 6) {
                g_connect_stat[g_local_core_id].tcp_del_stream_num++;
            } else if (flow->l4_proto == 17) {
                g_connect_stat[g_local_core_id].udp_del_stream_num++;
            }
            g_connect_stat[g_local_core_id].total_del_stream_num++;
            aclk_connect_timeout_update_v4(flow);
            cvmx_zone_free(g_connect_zone_v4, flow);

            memcpy(item, item_hd + hd->item_used - 1, sizeof(aclk_connect_hash_table_item_t));
            memset(item_hd + hd->item_used - 1, 0x00, sizeof(aclk_connect_hash_table_item_t));

            hd->item_used--;
        }
    }

    return;
}

void aclk_connect_timer_callback(aclk_connect_hash_table_info_t *table, uint32_t hash)
{
    uint8_t idx;
    uint64_t cycles;
    aclk_dpi_ipv4_flow_info_t *flow_v4;
#ifdef CONNECT_IPV6
    aclk_dpi_ipv6_flow_info_t *flow_v6;
#endif
    aclk_connect_hash_table_item_header_t *hd;
    aclk_connect_hash_table_item_t *item_hd, *item;

    if (hash < g_connect_table_v4->table_num) {
        idx = 0;
        cycles = cvmx_get_cycle();
        hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v4->table + g_connect_table_v4->table_size * hash);
        item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
        for (idx = 0; idx < hd->item_used; idx++) {
            item = item_hd + idx;
            flow_v4 = (aclk_dpi_ipv4_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
            //printf("cycles:%lu, cycles:%lu, timeout:%lu\n", cycles, flow->last_time, g_connect_cycle_timeout);
            if (cycles - flow_v4->last_time > g_connect_cycle_timeout) {
                ///flow timeout
                if (flow_v4->l4_proto == 6) {
                    g_connect_stat[g_local_core_id].tcp_del_stream_num++;
                } else if (flow_v4->l4_proto == 17) {
                    g_connect_stat[g_local_core_id].udp_del_stream_num++;
                }
                g_connect_stat[g_local_core_id].total_del_stream_num++;
                aclk_connect_timeout_update_v4(flow_v4);
                cvmx_zone_free(g_connect_zone_v4, flow_v4);

                memcpy(item, item_hd + hd->item_used - 1, sizeof(aclk_connect_hash_table_item_t));
                memset(item_hd + hd->item_used - 1, 0x00, sizeof(aclk_connect_hash_table_item_t));

                hd->item_used--;
            }
        }
    }

#ifdef CONNECT_IPV6
    if (hash < g_connect_table_v6->table_num) {
        idx = 0;
        cycles = cvmx_get_cycle();
        hd = (aclk_connect_hash_table_item_header_t *)(g_connect_table_v6->table + g_connect_table_v6->table_size * hash);
        item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
        for (idx = 0; idx < hd->item_used; idx++) {
            item = item_hd + idx;
            flow_v6 = (aclk_dpi_ipv6_flow_info_t *)cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
            //printf("cycles:%lu, cycles:%lu, timeout:%lu\n", cycles, flow->last_time, g_connect_cycle_timeout);
            if (cycles - flow_v6->last_time > g_connect_cycle_timeout) {
                ///flow timeout
                if (flow_v6->l4_proto == 6) {
                    g_connect_stat[g_local_core_id].tcp_del_stream_num++;
                } else if (flow_v6->l4_proto == 17) {
                    g_connect_stat[g_local_core_id].udp_del_stream_num++;
                }
                g_connect_stat[g_local_core_id].total_del_stream_num++;
                aclk_connect_timeout_update_v6(flow_v6);
                cvmx_zone_free(g_connect_zone_v6, flow_v6);

                memcpy(item, item_hd + hd->item_used - 1, sizeof(aclk_connect_hash_table_item_t));
                memset(item_hd + hd->item_used - 1, 0x00, sizeof(aclk_connect_hash_table_item_t));

                hd->item_used--;
            }
        }
    }
#endif

    return;
}

int aclk_connect_timer_create(uint32_t tag, uint8_t tt, uint8_t qos, uint8_t grp, uint32_t ticks, void *data, uint32_t len)
{
    cvmx_wqe_t *wqe;

    wqe = cvmx_fpa_alloc(CVMX_FPA_WQE_POOL);
    if (NULL == wqe) {
        aclk_uart_printf("%s[%d]:create timer packet error\n", __func__, __LINE__);
        return -1;
    }
    
    memset(wqe, 0x00, CVMX_FPA_WQE_POOL_SIZE);
    cvmx_wqe_set_unused8(wqe, ACLK_HAL_PACKET_UNSED_ID_TIMER);
    cvmx_wqe_set_tag(wqe, tag);
    cvmx_wqe_set_tt(wqe, tt);
    cvmx_wqe_set_qos(wqe, qos);
    cvmx_wqe_set_grp(wqe, grp);
    memcpy(wqe->packet_data, data, len);
    if (ticks) {
        if (cvmx_tim_add_entry(wqe, ticks, NULL)) {
            aclk_uart_printf("%s[%d]:tim add entry error\n", __func__, __LINE__);
            return -1;
        }
    } else {
        cvmx_pow_work_submit(wqe, tag, tt, qos, grp);
    }

    return 0;
}

int aclk_connect_process_timer(void *packet)
{
    cvmx_wqe_t *wqe;
    aclk_connect_timer_packet_t *tm;

    wqe = (cvmx_wqe_t *)packet;
    tm = (aclk_connect_timer_packet_t *)(wqe->packet_data);
    if (tm->magic_num != ACLK_TIMER_PACKET_MAGIC_NUM) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:Invalid timer packet\n", __func__, __LINE__);
        return -1;
    }
    //printf("process timer:%u\n", tm->cur_hash);
    
    if (tm->cb) {
        tm->cb(tm->table, tm->cur_hash);
    } else {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:tm cb is null\n", __func__, __LINE__);
    }
    ///updata tim packet data
    tm->cur_hash++;
    if (tm->cur_hash > tm->end_hash) {
        tm->cur_hash = tm->start_hash;
    }
    cvmx_wqe_set_tag(wqe, tm->cur_hash);

    ///add tm packet to tim queue
    if (tm->ticks) {
        if (cvmx_tim_add_entry(wqe, tm->ticks, NULL)) {
            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:cvmx add tim entry error\n", __func__, __LINE__);
            return -1;
        }
    } else {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "tm ticks is zero\n");
        cvmx_pow_work_submit(wqe, cvmx_wqe_get_tag(wqe), cvmx_wqe_get_tt(wqe), cvmx_wqe_get_qos(wqe), cvmx_wqe_get_grp(wqe));
    }

   return 0;
}

int aclk_connect_timer_init(void)
{
    unsigned int i, remainder, div, table_num;
    aclk_connect_timer_packet_t tm;

    tm.magic_num = ACLK_TIMER_PACKET_MAGIC_NUM;
    tm.ticks = CONNECT_TIMER_TICKS_2MS;
    tm.cb = aclk_connect_timer_callback;
    tm.table = g_connect_table_v4;
    table_num = (g_connect_table_v4->table_num > g_connect_table_v6->table_num) ? g_connect_table_v4->table_num : g_connect_table_v6->table_num;
    div = table_num / CONNECT_TIMER_NUM;
    remainder = table_num % CONNECT_TIMER_NUM;
    for (i = 0; i < CONNECT_TIMER_NUM; i++) {
        if (i < remainder) {
            tm.start_hash = i * (div + 1);
            tm.end_hash = tm.start_hash + div;
        } else {
            tm.start_hash = i * div + remainder;
            tm.end_hash = tm.start_hash + div - 1;
        }
        tm.cur_hash = tm.start_hash;
        if (aclk_connect_timer_create(tm.cur_hash, CVMX_POW_TAG_TYPE_ATOMIC, 0, 0, tm.ticks, (void *)(&tm), sizeof(aclk_connect_timer_packet_t))) {
            aclk_uart_printf("%s[%d]:aclk connect timer create error\n", __func__, __LINE__);
            return -1;
        }
    }

    return 0;
}
#endif

int aclk_connect_init_local(void)
{
    ///get current coreid 
    ///g_local_core_id = cvmx_get_core_num();

    return 0;
}

int aclk_connect_init_global(void)
{
    void *ptr;

    ///judge 
    if (sizeof(aclk_dpi_ipv4_flow_info_t) > ACLK_CONNECT_ZONE_V4_SIZE) {
        aclk_uart_printf("%s[%d]:create zone v6 from addr error\n", __func__, __LINE__);
        return -1;
    }
#ifdef CONNECT_IPV6
    if (sizeof(aclk_dpi_ipv6_flow_info_t) > ACLK_CONNECT_ZONE_V6_SIZE) {
        aclk_uart_printf("%s[%d]:create zone v6 from addr error\n", __func__, __LINE__);
        return -1;
    }
#endif

    ///g_connect_table_v4 = (aclk_connect_hash_table_info_t *)cvmx_bootmem_alloc_named(ACLK_CONNECT_TABLE_V4_NUM * ACLK_CONNECT_TABLE_V4_SIZE + sizeof(aclk_connect_hash_table_info_t), 128, ACLK_CONNECT_TABLE_IPV4_NAME);
    g_connect_table_v4 = (aclk_connect_hash_table_info_t *)malloc(ACLK_CONNECT_TABLE_V4_NUM * ACLK_CONNECT_TABLE_V4_SIZE + sizeof(aclk_connect_hash_table_info_t));
    if (NULL == g_connect_table_v4) {
        aclk_uart_printf("%s[%d]:aclk connect table v4 malloc error\n", __func__, __LINE__);
        return -1;
    }
    memset(g_connect_table_v4, 0x00, ACLK_CONNECT_TABLE_V4_NUM * ACLK_CONNECT_TABLE_V4_SIZE + sizeof(aclk_connect_hash_table_info_t));
    strcpy(g_connect_table_v4->name, ACLK_CONNECT_TABLE_IPV4_NAME);
    g_connect_table_v4->table_num = ACLK_CONNECT_TABLE_V4_NUM;
    g_connect_table_v4->table_size = ACLK_CONNECT_TABLE_V4_SIZE;
    g_connect_table_v4->item_num = (ACLK_CONNECT_TABLE_V4_SIZE - sizeof(aclk_connect_hash_table_item_header_t)) / sizeof(aclk_connect_hash_table_item_t);
    g_connect_table_v4->item_size = sizeof(aclk_connect_hash_table_item_t);
    g_connect_table_v4->compare = session_table_compare_cb_v4;
   
#if 0
    ///malloc for g_connect_zone_v4;
    ptr = cvmx_bootmem_alloc_named(ACLK_CONNECT_ZONE_V4_NUM * ACLK_CONNECT_ZONE_V4_SIZE + 128, 128, ACLK_CONNECT_ZONE_IPV4_NAME);
    if (ptr == NULL) {
        aclk_uart_printf("%s[%d]:aclk connect zone v4 malloc error\n", __func__, __LINE__);
        cvmx_bootmem_free_named(ACLK_CONNECT_TABLE_IPV4_NAME);

        return -1;
    }   
    g_connect_zone_v4 = cvmx_zone_create_from_addr(ACLK_CONNECT_ZONE_IPV4_NAME, ACLK_CONNECT_ZONE_V4_SIZE, ACLK_CONNECT_ZONE_V4_NUM,  ptr, ACLK_CONNECT_ZONE_V4_NUM * ACLK_CONNECT_ZONE_V4_SIZE + 128, 0); 
    if (g_connect_zone_v4 == NULL) {
        aclk_uart_printf("%s[%d]:create zone v4 from addr error\n", __func__, __LINE__);
        cvmx_bootmem_free_named(ACLK_CONNECT_TABLE_IPV4_NAME);
        cvmx_bootmem_free_named(ACLK_CONNECT_ZONE_IPV4_NAME);

        return -1;
    }
#endif

#ifdef CONNECT_IPV6
    ///g_connect_table_v6 = (aclk_connect_hash_table_info_t *)cvmx_bootmem_alloc_named(ACLK_CONNECT_TABLE_V6_NUM * ACLK_CONNECT_TABLE_V6_SIZE + sizeof(aclk_connect_hash_table_info_t), 128, ACLK_CONNECT_TABLE_IPV6_NAME);
    g_connect_table_v6 = (aclk_connect_hash_table_info_t *)malloc(ACLK_CONNECT_TABLE_V6_NUM * ACLK_CONNECT_TABLE_V6_SIZE + sizeof(aclk_connect_hash_table_info_t));
    if (NULL == g_connect_table_v6) {
        aclk_uart_printf("%s[%d]:aclk connect table v6 malloc error\n", __func__, __LINE__);
        return -1;
    }
    memset(g_connect_table_v6, 0x00, ACLK_CONNECT_TABLE_V6_NUM * ACLK_CONNECT_TABLE_V6_SIZE + sizeof(aclk_connect_hash_table_info_t));
    strcpy(g_connect_table_v6->name, ACLK_CONNECT_TABLE_IPV6_NAME);
    g_connect_table_v6->table_num = ACLK_CONNECT_TABLE_V6_NUM;
    g_connect_table_v6->table_size = ACLK_CONNECT_TABLE_V6_SIZE;
    g_connect_table_v6->item_num = (ACLK_CONNECT_TABLE_V6_SIZE - sizeof(aclk_connect_hash_table_item_header_t)) / sizeof(aclk_connect_hash_table_item_t);
    g_connect_table_v6->item_size = sizeof(aclk_connect_hash_table_item_t);
    g_connect_table_v6->compare = session_table_compare_cb_v6;
    
#if 0
    ///malloc for g_connect_zone_v6;
    ptr = cvmx_bootmem_alloc_named(ACLK_CONNECT_ZONE_V6_NUM * ACLK_CONNECT_ZONE_V6_SIZE + 128, 128, ACLK_CONNECT_ZONE_IPV6_NAME);
    if (ptr == NULL) {
        aclk_uart_printf("%s[%d]:aclk connect zone v6 malloc error\n", __func__, __LINE__);
        cvmx_bootmem_free_named(ACLK_CONNECT_TABLE_IPV6_NAME);
        cvmx_bootmem_free_named(ACLK_CONNECT_TABLE_IPV4_NAME);
        cvmx_bootmem_free_named(ACLK_CONNECT_ZONE_IPV4_NAME);

        return -1;
    }
    g_connect_zone_v6 = cvmx_zone_create_from_addr(ACLK_CONNECT_ZONE_IPV6_NAME, ACLK_CONNECT_ZONE_V6_SIZE, ACLK_CONNECT_ZONE_V6_NUM,  ptr, ACLK_CONNECT_ZONE_V6_NUM * ACLK_CONNECT_ZONE_V6_SIZE + 128, 0); 
    if (g_connect_zone_v6 == NULL) {
        aclk_uart_printf("%s[%d]:create zone v6 from addr error\n", __func__, __LINE__);
        cvmx_bootmem_free_named(ACLK_CONNECT_TABLE_IPV6_NAME);
        cvmx_bootmem_free_named(ACLK_CONNECT_ZONE_IPV6_NAME);
        cvmx_bootmem_free_named(ACLK_CONNECT_TABLE_IPV4_NAME);
        cvmx_bootmem_free_named(ACLK_CONNECT_ZONE_IPV4_NAME);

        return -1;
    }
#endif
#endif

#if 0
    if (aclk_connect_timer_init()) {
        aclk_uart_printf("%s[%d]:aclk connect timer init error\n", __func__, __LINE__);
        return -1;
    }
#endif

    memset(g_connect_stat, 0x00, sizeof(g_connect_stat));
    ///g_connect_cycle_timeout = DEFAULT_CONNECT_TIMEOUT * ((uint64_t)(cvmx_sysinfo_get()->cpu_clock_hz));
    ///aclk_uart_printf("timeout cycle:%lu\n", g_connect_cycle_timeout);
    ///register connect uart command
    ///aclk_shell_cmd_register("connect", "connect command", aclk_connect_cmd);

    return 0;
}

void *connect_table_search(aclk_connect_hash_table_info_t *table, uint32_t hash, void *item_a)
{
    uint8_t idx;
    void *item_b;
    aclk_connect_hash_table_item_header_t *hd;
    aclk_connect_hash_table_item_t *item_hd, *item;
    ///flow_five_tuple_v4_t *src, *dst;

    if ((NULL == table) || (NULL == item_a)) {
        return NULL;
    }
    if (hash >= table->table_num) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:Invalid hash value %u(max:%u)\n", __func__, __LINE__, hash, table->table_num);
        return NULL;
    }

    idx = 0;
    item_b = NULL;
    hd = (aclk_connect_hash_table_item_header_t *)(table->table + table->table_size * hash);
    item_hd = (aclk_connect_hash_table_item_t *)(hd + 1);
    for (idx = 0; idx < hd->item_used; idx++) {
        item = item_hd + idx;
        item_b = cvmx_phys_to_ptr(((uint64_t)(item->flow)) << ACLK_CONNECT_ADDR_SHIFT);
        if (0 == table->compare(item_a, item_b)) {
            return item_b;
        ///} else {
        ///    if (item_b) {
        ///        dst = (flow_five_tuple_v4_t *)item_b;
        ///        printf("%s[%u]: itme_b,sip:%x, dip:%x, sport:%u, dport:%u, proto:%u\n", __func__, __LINE__, dst->sip, dst->dip, dst->sport, dst->dport, dst->l4_proto);
        ///    } else {
        ///        printf("item_b is null\n");
        ///    }
        ///    src = (flow_five_tuple_v4_t *)item_a;
        ///    printf("%s[%u]: itme_a,sip:%x, dip:%x, sport:%u, dport:%u, proto:%u\n", __func__, __LINE__, src->sip, src->dip, src->sport, src->dport, src->l4_proto);
        }
    }
    
    ///printf("hd:%p, item_hd:%p\n", hd, item_hd);
    ///printf("item used:%dm, %p, %p\n", hd->item_used, item_a, item_b);

    return NULL;
}

int connect_table_insert(aclk_connect_hash_table_info_t *table, uint32_t hash, void *item_a)
{
    uint8_t idx;
    aclk_connect_hash_table_item_header_t *hd;
    aclk_connect_hash_table_item_t *item;

    if ((NULL == table) || (NULL == item_a)) {
        return -1;
    }
    if (hash >= table->table_num) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:Invalid hash value %u(max:%u)\n", __func__, __LINE__, hash, table->table_num);
        return -1;
    }

    idx = 0;
    hd = (aclk_connect_hash_table_item_header_t *)(table->table + table->table_size * hash);
    item = (aclk_connect_hash_table_item_t *)(hd + 1);
    if (hd->item_used < table->item_num) {
        item = item + hd->item_used;
        item->flow = cvmx_ptr_to_phys(item_a) >> ACLK_CONNECT_ADDR_SHIFT;
        hd->item_used++;
    }

    return 0;
}

int aclk_connect_process_packet_v4(void *packet)
{
    uint32_t hash;
    uint64_t cycles;
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;
    flow_five_tuple_v4_t tuple;
    aclk_dpi_ipv4_flow_info_t *flow = NULL;

    ///get packet info
    wqe = (cvmx_wqe_t *)packet;
    pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    if ((pkt->sip.ipv4 < pkt->dip.ipv4) || ((pkt->sip.ipv4 == pkt->dip.ipv4) && (pkt->sport <= pkt->dport))) {
        tuple.sip = pkt->sip.ipv4;
        tuple.dip = pkt->dip.ipv4;
        tuple.sport = pkt->sport;
        tuple.dport = pkt->dport;
        tuple.l4_proto = pkt->l4_proto;
    } else {
        tuple.sip = pkt->dip.ipv4;
        tuple.dip = pkt->sip.ipv4;
        tuple.sport = pkt->dport;
        tuple.dport = pkt->sport;
        tuple.l4_proto = pkt->l4_proto;
    }
    hash = get_five_tuple_v4_hash(tuple.sip, tuple.dip, tuple.sport, tuple.dport, tuple.l4_proto);
    pkt->flow_hash = hash;

    cycles = cvmx_get_cycle();
    hash = hash & ACLK_CONNECT_TABLE_V4_MASK;
    //cvmx_pow_tag_sw(hash, CVMX_POW_TAG_TYPE_ATOMIC);
    cvmx_pow_tag_sw_full((cvmx_wqe_t *)0x80, hash, CVMX_POW_TAG_TYPE_ATOMIC, 0);
    cvmx_pow_tag_sw_wait();
    flow = connect_table_search(g_connect_table_v4, hash, (void *)&tuple);
    if (flow) {
        ///flow already exist
        //update flow info
        //flow->seq++;
        if (cycles - flow->last_time > g_connect_cycle_timeout) {
            ///flow timeout
            aclk_pide_flow_timeout_stat(flow->appidx, flow->seq, flow->bytes);
            if (flow->l4_proto == 6) {
                g_connect_stat[g_local_core_id].tcp_del_stream_num++;
            } else if (flow->l4_proto == 17) {
                g_connect_stat[g_local_core_id].udp_del_stream_num++;
            }
            g_connect_stat[g_local_core_id].total_del_stream_num++;
            
            ///use same space
            pkt->flow_dir = session_flow_dir(packet);
            pkt->seq_no = 1;
            if ((flow->sip == pkt->sip.ipv4) && (flow->sport == pkt->sport)) {
                flow->dir = pkt->flow_dir;
            } else {
                flow->dir = DOWN_FLOW;
            }
            flow->seq = pkt->seq_no;
            flow->bytes = cvmx_wqe_get_len(wqe);
            flow->appidx = 0;   //unkown
            flow->start_time = cycles;
            flow->last_time = cycles;
            
            pkt->flow = flow;
            if (flow->l4_proto == 6) {
                g_connect_stat[g_local_core_id].tcp_add_stream_num++;
            } else if (flow->l4_proto == 17) {
                g_connect_stat[g_local_core_id].udp_add_stream_num++;
            }
            g_connect_stat[g_local_core_id].total_add_stream_num++;
        } else {
            if ((flow->seq == 0xffff) || (flow->bytes == 0xffffffff)) {
                aclk_pide_flow_timeout_stat(flow->appidx, flow->seq, flow->bytes);            
                flow->seq = 0;
                flow->bytes = 0;
            }
            flow->seq++;
            flow->bytes += cvmx_wqe_get_len(wqe);
            flow->last_time = cvmx_get_cycle();
            pkt->appidx = flow->appidx;
            pkt->flow = flow;
            pkt->seq_no = flow->seq;
            if ((flow->sip == pkt->sip.ipv4) && (flow->sport == pkt->sport)) {
                pkt->flow_dir = flow->dir;
            } else {
                if (UP_FLOW == flow->dir) {
                    pkt->flow_dir = DOWN_FLOW;
                } else {
                    pkt->flow_dir = UP_FLOW;
                }
            }
        }
    } else {
        ///flow need create
        flow = cvmx_zone_alloc(g_connect_zone_v4, 0);
        if (NULL == flow) {
            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: malloc for flow error\n", __func__, __LINE__);
            return -1;
        }
        memset(flow, 0x00, sizeof(flow));
        flow->sip = pkt->sip.ipv4;
        flow->dip = pkt->dip.ipv4;
        flow->sport = pkt->sport;
        flow->dport = pkt->dport;
        flow->l4_proto = pkt->l4_proto;
        flow->dir = session_flow_dir(packet);
        flow->seq = 1;
        flow->bytes = cvmx_wqe_get_len(wqe);
        flow->appidx = 0;   //unkown
        flow->start_time = flow->last_time = cvmx_get_cycle();
        if (connect_table_insert(g_connect_table_v4, hash, (void *)flow)) {
            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: malloc for flow error\n", __func__, __LINE__);
            cvmx_zone_free(g_connect_zone_v4, flow);

            return -1;
        }
        pkt->flow = flow;
        ///first packet:up flow packet
        pkt->flow_dir = flow->dir;
        pkt->seq_no = flow->seq;

        if (flow->l4_proto == 6) {
            g_connect_stat[g_local_core_id].tcp_add_stream_num++;
        } else if (flow->l4_proto == 17) {
            g_connect_stat[g_local_core_id].udp_add_stream_num++;
        }
        g_connect_stat[g_local_core_id].total_add_stream_num++;
    }
    ///cvmx_pow_tag_sw_null();
    if (flow->l4_proto == 6) {
        g_connect_stat[g_local_core_id].tcp_add_packet_num++;
        g_connect_stat[g_local_core_id].tcp_add_bytes += cvmx_wqe_get_len(wqe);
    } else if (flow->l4_proto == 17) {
        g_connect_stat[g_local_core_id].udp_add_packet_num++;
        g_connect_stat[g_local_core_id].udp_add_bytes += cvmx_wqe_get_len(wqe);
    }
    g_connect_stat[g_local_core_id].total_add_packet_num++;
    g_connect_stat[g_local_core_id].total_add_bytes += cvmx_wqe_get_len(wqe);

    return 0;
}

int aclk_connect_process_packet_v6(void *packet)
{
    int recode;
    uint32_t hash;
    uint64_t cycles;
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;
    flow_five_tuple_v6_t tuple;
    aclk_dpi_ipv6_flow_info_t *flow = NULL;

    ///get packet info
    wqe = (cvmx_wqe_t *)packet;
    pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    recode = memcmp(pkt->sip.ipv6, pkt->dip.ipv6, 16);
    if ((recode > 0) || ((recode == 0) && (pkt->sport < pkt->dport))) {
        memcpy(tuple.sip, pkt->sip.ipv6, 16);
        memcpy(tuple.dip, pkt->dip.ipv6, 16);
        tuple.sport = pkt->sport;
        tuple.dport = pkt->dport;
        tuple.l4_proto = pkt->l4_proto;
    } else {
        memcpy(tuple.sip, pkt->dip.ipv6, 16);
        memcpy(tuple.dip, pkt->sip.ipv6, 16);
        tuple.sport = pkt->dport;
        tuple.dport = pkt->sport;
        tuple.l4_proto = pkt->l4_proto;
    }
    hash = get_five_tuple_v6_hash(tuple.sip, tuple.dip, tuple.sport, tuple.dport, tuple.l4_proto);
    pkt->flow_hash = hash;

    cycles = cvmx_get_cycle();
    hash = hash & ACLK_CONNECT_TABLE_V6_MASK;
    //cvmx_pow_tag_sw(hash, CVMX_POW_TAG_TYPE_ATOMIC);
    cvmx_pow_tag_sw_full((cvmx_wqe_t *)0x80, hash, CVMX_POW_TAG_TYPE_ATOMIC, 0);
    cvmx_pow_tag_sw_wait();
    flow = connect_table_search(g_connect_table_v6, hash, (void *)&tuple);
    if (flow) {
        ///flow already exist
        if (cycles - flow->last_time > g_connect_cycle_timeout) {
            ///flow timeout
            aclk_pide_flow_timeout_stat(flow->appidx, flow->seq, flow->bytes);
            if (flow->l4_proto == 6) {
                g_connect_stat[g_local_core_id].tcp_del_stream_num++;
            } else if (flow->l4_proto == 17) {
                g_connect_stat[g_local_core_id].udp_del_stream_num++;
            }
            g_connect_stat[g_local_core_id].total_del_stream_num++;
            
            ///use same space
            pkt->flow_dir = session_flow_dir(packet);
            pkt->seq_no = 1;
            if ((0 == memcmp(flow->sip, pkt->sip.ipv6, 16)) && (flow->sport == pkt->sport)) {
                flow->dir = pkt->flow_dir;
            } else {
                flow->dir = DOWN_FLOW;
            }
            flow->seq = pkt->seq_no;
            flow->bytes = cvmx_wqe_get_len(wqe);
            flow->appidx = 0;   //unkown
            flow->start_time = cycles;
            flow->last_time = cycles;
            
            pkt->flow = flow;
            if (flow->l4_proto == 6) {
                g_connect_stat[g_local_core_id].tcp_add_stream_num++;
            } else if (flow->l4_proto == 17) {
                g_connect_stat[g_local_core_id].udp_add_stream_num++;
            }
            g_connect_stat[g_local_core_id].total_add_stream_num++;
        } else {
            //update flow info
            if ((flow->seq == 0xffff) 
                    || (flow->bytes >= (0xffffffff - cvmx_wqe_get_len(wqe)))) {
                aclk_pide_flow_timeout_stat(flow->appidx, flow->seq, flow->bytes);            
                flow->seq = 0;
                flow->bytes = 0;
            }
            flow->seq++;
            flow->bytes += cvmx_wqe_get_len(wqe);
            flow->last_time = cvmx_get_cycle();
            pkt->appidx = flow->appidx;
            pkt->flow = flow;
            if ((0 == memcmp(flow->sip, pkt->sip.ipv6, 16)) && (flow->sport == pkt->sport)) {
                pkt->flow_dir = flow->dir;
            } else {
                if (UP_FLOW == flow->dir) {
                    pkt->flow_dir = DOWN_FLOW;
                } else {
                    pkt->flow_dir = UP_FLOW;
                }
            }
        }
    } else {
        ///flow need create
        flow = cvmx_zone_alloc(g_connect_zone_v6, 0);
        if (NULL == flow) {
            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: malloc for flow error\n", __func__, __LINE__);
            return -1;
        }
        memset(flow, 0x00, sizeof(flow));
        memcpy(flow->sip, tuple.sip, 16);
        memcpy(flow->dip, tuple.dip, 16);
        flow->sport = tuple.sport;
        flow->dport = tuple.dport;
        flow->l4_proto = pkt->l4_proto;
        flow->dir = session_flow_dir(packet);
        flow->seq = 1;
        flow->bytes = cvmx_wqe_get_len(wqe);
        flow->appidx = 0;   //unkown
        flow->start_time = flow->last_time = cvmx_get_cycle();
        if (connect_table_insert(g_connect_table_v6, hash, (void *)flow)) {
            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: malloc for flow error\n", __func__, __LINE__);
            cvmx_zone_free(g_connect_zone_v6, flow);

            return -1;
        }
        pkt->flow = flow;
        pkt->flow_dir = flow->dir;

        if (flow->l4_proto == 6) {
            g_connect_stat[g_local_core_id].tcp_add_stream_num++;
        } else if (flow->l4_proto == 17) {
            g_connect_stat[g_local_core_id].udp_add_stream_num++;
        }
        g_connect_stat[g_local_core_id].total_add_stream_num++;
    }
    ///cvmx_pow_tag_sw_null();
    if (flow->l4_proto == 6) {
        g_connect_stat[g_local_core_id].tcp_add_packet_num++;
        g_connect_stat[g_local_core_id].tcp_add_bytes += cvmx_wqe_get_len(wqe);
    } else if (flow->l4_proto == 17) {
        g_connect_stat[g_local_core_id].udp_add_packet_num++;
        g_connect_stat[g_local_core_id].udp_add_bytes += cvmx_wqe_get_len(wqe);
    }
    g_connect_stat[g_local_core_id].total_add_packet_num++;
    g_connect_stat[g_local_core_id].total_add_bytes += cvmx_wqe_get_len(wqe);

    return 0;
}

int aclk_connect_process_packet(void *packet)
{
    cvmx_wqe_t *wqe;
    aclk_dpi_pkt_info_t *pkt;

    if (NULL == packet) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid packet ptr\n", __func__, __LINE__);
        return -1;
    }
    
    ///get packet info
    wqe = (cvmx_wqe_t *)packet;
    pkt = (aclk_dpi_pkt_info_t *)(wqe->packet_data);
    if (4 == pkt->ip_ver) {
        ///printf("ipv4, sip:%x, dip:%x, sport:%u, dport:%u,proto:%u\n", pkt->sip.ipv4, pkt->dip.ipv4, pkt->sport, pkt->dport, pkt->l4_proto);
        if (aclk_connect_process_packet_v4(packet)) {
            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: process ipv4 packet error\n", __func__, __LINE__);
            g_connect_stat[g_local_core_id].drop_packet_num++;
            g_connect_stat[g_local_core_id].drop_bytes += cvmx_wqe_get_len(wqe);
            aclk_dpi_process_packet_over(packet);

            return -1;
        }
    } else if (6 == pkt->ip_ver) {
#ifdef CONNECT_IPV6
        if (aclk_connect_process_packet_v6(packet)) {
            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: process ipv6 packet error\n", __func__, __LINE__);
            g_connect_stat[g_local_core_id].drop_packet_num++;
            g_connect_stat[g_local_core_id].drop_bytes += cvmx_wqe_get_len(wqe);
            aclk_dpi_process_packet_over(packet);

            return -1;
        }
#else
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: only support process ipv4 packet\n", __func__, __LINE__);
        return -1;
#endif
    } else {
        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]: invalid ip packet\n", __func__, __LINE__);
        g_connect_stat[g_local_core_id].drop_packet_num++;
        g_connect_stat[g_local_core_id].drop_bytes += cvmx_wqe_get_len(wqe);
        
        return -1;
    }
    
    return 0;
}

int aclk_connect_process_command(void *packet, void *data)
{
    int ret;
    uint32_t ssize;
    uint8_t sdata[BUFSIZ];
    aclk_dpi_cmd_t *req, *res;
    //unsigned int i;

    ret = 0;
    ssize = 0;
    req = (aclk_dpi_cmd_t *)data;
    res = (aclk_dpi_cmd_t *)sdata;
    switch (req->opcode & ACLK_MINOR_MASK) {
    case ACLK_CONNECT_CMD_SHOW_COUNTER:
        //printf("connect show counter %u\n", req->len);
        ssize = strlen("connect show counter\n");
        strcpy((char *)(res->data), "connect show counter\n");
        break;
    case ACLK_CONNECT_CMD_SHOW_TABLE_INFO:
        printf("connect show table info\n");
        ssize = strlen("connect show table info\n");
        strcpy((char *)(res->data), "connect show table info\n");
        break;
    default:
        ret = -1;
        break;
    }

    res->magic_num = ACLK_CMD_MAGIC_NUM;
    res->opcode = req->opcode | ACLK_RESPONSE_MASK;
    res->recode = ret;
    res->len = ssize;
    aclk_dpi_send_response(packet, sdata, ssize + sizeof(aclk_dpi_cmd_t));

    return 0;

    return ret;
}

void aclk_connect_fini_local(void)
{
    return;
}

void aclk_connect_fini_global(void)
{
    return;
}

