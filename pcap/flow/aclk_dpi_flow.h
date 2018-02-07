/**
 * @File Name: aclk_dpi_flow.h
 * @brief: 
 * @Author: zhangmeiqing
 * @Version: 1.0.0
 * @Date: Tue 03 May 2016 02:26:21 PM CST
 */
#ifndef __ACLK_DPI_CONNECT_H__
#define __ACLK_DPI_CONNECT_H__

#ifdef  __cpulsplus
extern "C" {
#endif


#define CONNECT_IPV6    1

#define ACLK_CONNECT_TABLE_IPV4_NAME    "ipv4_connect_table"
#define ACLK_CONNECT_ZONE_IPV4_NAME     "ipv4_connect_zone"
#define ACLK_CONNECT_TABLE_IPV6_NAME    "ipv6_connect_table"
#define ACLK_CONNECT_ZONE_IPV6_NAME     "ipv6_connect_zone"

///used for crc hash
#define POLY        0x1edc6f41
#define BIT_WIDTH_V4   22
#define BIT_WIDTH_V6   20

#define ACLK_CONNECT_TABLE_V4_NUM           (1lu << BIT_WIDTH_V4)
#define ACLK_CONNECT_TABLE_V6_NUM           (1lu << BIT_WIDTH_V6)
#define ACLK_CONNECT_TABLE_V4_MASK          (ACLK_CONNECT_TABLE_V4_NUM - 1)
#define ACLK_CONNECT_TABLE_V6_MASK          (ACLK_CONNECT_TABLE_V6_NUM - 1)
#define ACLK_CONNECT_TABLE_V4_SIZE          128
#define ACLK_CONNECT_TABLE_V6_SIZE          64
#define ACLK_CONNECT_ZONE_V4_NUM            (ACLK_CONNECT_TABLE_V4_NUM * 2)
#define ACLK_CONNECT_ZONE_V6_NUM            (ACLK_CONNECT_TABLE_V6_NUM * 2)
#define ACLK_CONNECT_ZONE_V4_SIZE           128
#define ACLK_CONNECT_ZONE_V6_SIZE           256

#define ACLK_CONNECT_ADDR_SHIFT             7

typedef enum {
    ACLK_CONNECT_CMD_SHOW_COUNTER = 1,
    ACLK_CONNECT_CMD_SHOW_TABLE_INFO,
} aclk_connect_cmd_t;

typedef int (* flow_compare_cb)(void *item_a, void *item_b);

typedef struct {
    uint32_t next;  ///next bucket phy addr << 7
    uint8_t  item_used;
    uint8_t  reserved[3];
} aclk_connect_hash_table_item_header_t;

typedef struct {
    uint32_t flow;      /// zone phy addr << 7
    uint32_t reserved;
} aclk_connect_hash_table_item_t;

typedef struct {
    char name[32];
    uint32_t table_num;
    uint16_t table_size;
    uint8_t  item_num;
    uint8_t  item_size;
    flow_compare_cb compare;
    uint8_t  table[0];
} aclk_connect_hash_table_info_t;

typedef struct {
    uint32_t sip;
    uint32_t dip;
    uint16_t  sport;
    uint16_t  dport;
    uint8_t   l4_proto;
} flow_five_tuple_v4_t;

typedef struct {
    uint16_t sip[8];
    uint16_t dip[8];
    uint16_t  sport;
    uint16_t  dport;
    uint8_t   l4_proto;
} flow_five_tuple_v6_t;

typedef enum {
    UP_FLOW = 1,
    DOWN_FLOW,
} flow_direction_t;

typedef struct {
    uint32_t sip;
    uint32_t dip;

    uint16_t sport;
    uint16_t dport;
    uint8_t  l4_proto;
    uint8_t  dir;
    uint16_t seq;
    
    uint32_t appidx;
    uint32_t bytes;

    uint64_t start_time;
    uint64_t last_time;
} aclk_dpi_ipv4_flow_info_t;

typedef struct {
    uint16_t sip[8];
    uint16_t dip[8];

    uint16_t sport;
    uint16_t dport;
    uint8_t  l4_proto;
    uint8_t  dir;
    uint16_t seq;
    
    uint32_t appidx;
    uint32_t bytes;

    uint64_t start_time;
    uint64_t last_time;
} aclk_dpi_ipv6_flow_info_t;

typedef struct {
    uint64_t    tcp_add_stream_num;
    uint64_t    tcp_del_stream_num;
    uint64_t    tcp_add_packet_num;
    uint64_t    tcp_add_bytes;
    uint64_t    udp_add_stream_num;
    uint64_t    udp_del_stream_num;
    uint64_t    udp_add_packet_num;
    uint64_t    udp_add_bytes;
    uint64_t    total_add_stream_num;
    uint64_t    total_del_stream_num;
    uint64_t    total_add_packet_num;
    uint64_t    total_add_bytes;
    uint64_t    drop_packet_num;
    uint64_t    drop_bytes;
} aclk_dpi_connect_stat_t;

int aclk_connect_init_local(void);
int aclk_connect_init_global(void);

int aclk_connect_process_packet(void *packet);
int aclk_connect_process_command(void *packet, void *data);

void aclk_connect_fini_local(void);
void aclk_connect_fini_global(void);

///connect timer
#define  ACLK_TIMER_PACKET_MAGIC_NUM    0x5aa5a55a
///connect table timeout
#define MAX_CONNECT_TIMEOUT         180        /// sec
#define DEFAULT_CONNECT_TIMEOUT     120         /// sec
#define MIN_CONNECT_TIMEOUT         30          /// sec

#define CONNECT_TIMER_NUM           1024
#define CONNECT_TIMER_TICKS_10MS    (10 * CVMX_TIM_TICKS_PER_MS)
#define CONNECT_TIMER_TICKS_2MS     (2 * CVMX_TIM_TICKS_PER_MS)

typedef void (*connect_timer_cb)(aclk_connect_hash_table_info_t *table, uint32_t hash);
typedef struct {
    uint32_t magic_num;
    uint32_t start_hash;
    uint32_t end_hash;
    uint32_t cur_hash;
    uint64_t ticks;
    connect_timer_cb cb;
    aclk_connect_hash_table_info_t *table;
} aclk_connect_timer_packet_t;

int aclk_connect_process_timer(void *packet);

void aclk_connect_get_match_result(void *stat);
void aclk_connect_clear_match_all_result(void);
void aclk_connect_clear_match_result(uint32_t appidx);

#ifdef  __cpulsplus
}
#endif

#endif


