/*
 * @Filename: aclk_hash_table.h
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 01/23/2017 03:44:31 PM
 */
#ifndef ACLK_HASH_TABLE_H__
#define ACLK_HASH_TABLE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ACLK_DPI_ADDR_SHIFT    7

typedef int (* item_compare_func)(void *item_a, void *item_b);
typedef void (* item_copy_func)(void *item_a, void *item_b);
typedef int (* item_timeout_compare_func)(void *item_a);
typedef void (* item_time_update_func)(void *item_a);

typedef enum {
    ACLK_HASH_TABLE_LOCK_SPIN,
    ACLK_HASH_TABLE_LOCK_TAG,
} aclk_hash_table_lock_t;

typedef struct {
    uint64_t next_addr:56;
    uint64_t item_used:8;
} aclk_hash_table_hdr_t;

typedef struct {
    void *addr;
} aclk_hash_table_item_t;

typedef struct {
    cvmx_spinlock_t lock;
    uint64_t next:56;
    uint64_t used:8;
    uint8_t item[0];
} aclk_hash_table_item_hdr_t;


/**
 *hash table header
 */
typedef struct {
    char name[32];
    uint32_t table_num;
    uint8_t table_size;
    uint8_t  item_num;
    uint8_t  item_size;
    aclk_hash_table_lock_t lock;    ///table item lock
    cvmx_zone_t table_zone;         ///table item zone, each have more than one items
    cvmx_zone_t item_zone;          ///table item info zone
    item_compare_func compare;      ///table item info compare callback
    item_copy_func copy;            ///table item info compare callback
    item_timeout_compare_func timeout;
    item_time_update_func update;
    uint8_t  table[0];
} aclk_hash_table_info_t;


aclk_hash_table_info_t *aclk_hash_table_create(char *table_name, uint32_t table_size, uint32_t table_num, uint32_t item_size, cvmx_zone_t table_zone, cvmx_zone_t item_zone, item_compare_func compare, item_copy_func copy, item_timeout_compare_func timeout, item_time_update_func update, aclk_hash_table_lock_t lock);
void aclk_hash_table_destroy(aclk_hash_table_info_t *table);

/**
 * @brief aclk_hash_table_find 
 *
 * @param table
 * @param hash
 * @param item
 *
 * @return if not find, return -1, else return 0 and store info in item.
 */
int aclk_hash_table_find(aclk_hash_table_info_t *table, uint32_t hash, void *item);

int aclk_hash_table_add(aclk_hash_table_info_t *table, uint32_t hash, void *item);
int aclk_hash_table_del(aclk_hash_table_info_t *table, uint32_t hash, void *item);


///int aclk_hash_table_fini(void);
#ifdef __cplusplus
}
#endif

#endif
