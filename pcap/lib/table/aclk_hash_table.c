/**
 * @Filename: aclk_hash_table.c
 * @Brief   :
 * @Author  : zhang.meiqing 
 * @Version : 1.0.0
 * @Date    : 01/24/2017 10:21:57 AM
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>

#include "aclk_sdk.h"
#include "aclk.h"

int aclk_hash_table_lock(aclk_hash_table_info_t *table, uint32_t hash)
{
    aclk_hash_table_item_hdr_t *hdr;

    if (hash > table->table_num) {
        return -1;
    }
    hdr = (aclk_hash_table_item_hdr_t *)(table->table + table->table_size * hash);
    switch (table->lock) {
        case ACLK_HASH_TABLE_LOCK_SPIN:
            cvmx_spinlock_lock(&(hdr->lock));
            break;
        case ACLK_HASH_TABLE_LOCK_TAG:
            cvmx_pow_tag_sw_full((cvmx_wqe_t *)0x80, hash, CVMX_POW_TAG_TYPE_ATOMIC, 0);
            break;
        default:
            break;
    }
   
    return 0;
}

int aclk_hash_table_unlock(aclk_hash_table_info_t *table, uint32_t hash)
{
    aclk_hash_table_item_hdr_t *hdr;

    if (hash > table->table_num) {
        return -1;
    }
    hdr = (aclk_hash_table_item_hdr_t *)(table->table + table->table_size * hash);
    switch (table->lock) {
        case ACLK_HASH_TABLE_LOCK_SPIN:
            cvmx_spinlock_unlock(&(hdr->lock));
            break;
        case ACLK_HASH_TABLE_LOCK_TAG:
            break;
        default:
            break;
    }
   
    return 0;
}

aclk_hash_table_info_t *aclk_hash_table_create(char *table_name, uint32_t table_size, uint32_t table_num, uint32_t item_size, cvmx_zone_t table_zone, cvmx_zone_t item_zone, item_compare_func compare, item_copy_func copy,item_timeout_compare_func timeout, item_time_update_func update, aclk_hash_table_lock_t lock)
{
    unsigned int i, size;
    aclk_hash_table_info_t *table;
    aclk_hash_table_item_hdr_t *hdr;

    size = table_size * table_num + sizeof(aclk_hash_table_info_t);
    table = (aclk_hash_table_info_t *)cvmx_bootmem_alloc_named(size, CVMX_CACHE_LINE_SIZE, table_name);
    if (NULL == table) {
        aclk_printf(ACLK_DPI_LOG_LEVEL_ERROR, "%s[%d]:malloc for table error\n", __FILE__, __LINE__);
        return NULL;
    }
    memset(table, 0x00, size);
    strcpy(table->name, table_name);
    table->table_num = table_num;
    table->table_size = table_size;
    table->item_num = (table_size - sizeof(aclk_hash_table_item_hdr_t)) / sizeof(aclk_hash_table_item_t);
    table->item_size = item_size;
    table->lock = lock;
    table->table_zone = table_zone;
    table->item_zone = item_zone;
    table->compare = compare;
    table->copy = copy;
    table->update = update;
    table->timeout = timeout;

    for (i = 0; i < table_num; i++) {
        hdr = (aclk_hash_table_item_hdr_t *)(table->table + table_size * i);
        switch (lock) {
            case ACLK_HASH_TABLE_LOCK_SPIN:
                cvmx_spinlock_init(&(hdr->lock));
                break;
            case ACLK_HASH_TABLE_LOCK_TAG:
                break;
            default:
                break;
        }
    }

    return table;
}

void aclk_hash_table_destroy(aclk_hash_table_info_t *table)
{
    unsigned int i, j;
    void *addr;
    aclk_hash_table_item_hdr_t *hdr;
    aclk_hash_table_item_t *item;

    for (i = 0; i < table->table_num; i++) {
        aclk_hash_table_lock(table, i);
        hdr = (aclk_hash_table_item_hdr_t *)(table->table + table->table_size * i);
        for (j = 0; j < hdr->used; j++) {
            item = (aclk_hash_table_item_t *)(hdr->item + sizeof(aclk_hash_table_item_t) * j);
            cvmx_zone_free(table->item_zone, item->addr);
        }
        if (hdr->next) {
            hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
            while (hdr) {
                ///storage addr->next 
                for (j = 0; j < hdr->used; j++) {
                    item = (aclk_hash_table_item_t *)(hdr->item + sizeof(aclk_hash_table_item_t) * j);
                    cvmx_zone_free(table->item_zone, item->addr);
                }
                addr = hdr;
                if (hdr->next) {
                    hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
                    cvmx_zone_free(table->table_zone, addr);
                } else {
                    cvmx_zone_free(table->table_zone, addr);
                    break;
                }
            }
        }
        aclk_hash_table_unlock(table, i);
    }
    cvmx_bootmem_free_named(table->name);

    return;
}

int aclk_hash_table_find(aclk_hash_table_info_t *table, uint32_t hash, void *info)
{
    int i;
    aclk_hash_table_item_hdr_t *hdr;
    aclk_hash_table_item_t *item;

    if (hash > table->table_num) {
        return -1;
    }

    aclk_hash_table_lock(table, hash);
    hdr = (aclk_hash_table_item_hdr_t *)(table->table + table->table_size * hash);
    for (i = 0; i < hdr->used; i++) {
        item = (aclk_hash_table_item_t *)(hdr->item + table->item_size * i);
        if ((0 == table->compare(item->addr, info)) && (0 == table->timeout(item->addr))) {
            table->update(item->addr);
            table->copy(info, item->addr);
            aclk_hash_table_unlock(table, hash);
            return 0;
        }
    }

    if (hdr->next) {
        hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
        while (hdr) {
            for (i = 0; i < hdr->used; i++) {
                item = (aclk_hash_table_item_t *)(hdr->item + table->item_size * i);
                if ((0 == table->compare(item->addr, info)) && (0 == table->timeout(item->addr))) {
                    table->update(item->addr);
                    table->copy(info, item->addr);
                    aclk_hash_table_unlock(table, hash);
                    return 0;
                }
            }
            
            if (hdr->next) {
                hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
            } else {
                break;
            }
        }
    }
    aclk_hash_table_unlock(table, hash);
   
    return -1;
}

int aclk_hash_table_add(aclk_hash_table_info_t *table, uint32_t hash, void *info)
{
    void *ptr;
    aclk_hash_table_item_hdr_t *hdr;
    aclk_hash_table_item_t *item;

    if (hash > table->table_num) {
        return -1;
    }

    aclk_hash_table_lock(table, hash);
    hdr = (aclk_hash_table_item_hdr_t *)(table->table + table->table_size * hash);
    if (hdr->used < table->item_num) {
        ptr = cvmx_zone_alloc(table->item_zone, 0);
        if (NULL == ptr) {
            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:malloc for item zone error\n", __func__, __LINE__);
            return -1;
        }
        memcpy(ptr, info, table->item_size);
        item = (aclk_hash_table_item_t *)(hdr->item + table->item_size * hdr->used);
        item->addr = ptr;
        hdr->used++;
    } else {
        if (hdr->next) {
            hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
            while (hdr) {
                if (hdr->used < table->item_num) {
                    ptr = cvmx_zone_alloc(table->item_zone, 0);
                    if (NULL == ptr) {
                        aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:malloc for item zone error\n", __func__, __LINE__);
                        return -1;
                    }
                    memcpy(ptr, info, table->item_size);
                    item = (aclk_hash_table_item_t *)(hdr->item + table->item_size * hdr->used);
                    item->addr = ptr;
                    hdr->used++;
                    break;
                } else {
                    if (hdr->next) {
                        hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
                    } else {
                        ///malloc table item;
                        hdr = (aclk_hash_table_item_hdr_t *)cvmx_zone_alloc(table->table_zone, 0);
                        if (NULL == hdr) {
                            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:malloc for table zone error\n", __func__, __LINE__);
                            return -1;
                        }
                        memset(hdr, 0x00, table->table_size);
                        ptr = cvmx_zone_alloc(table->item_zone, 0);
                        if (NULL == ptr) {
                            aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:malloc for item zone error\n", __func__, __LINE__);
                            return -1;
                        }
                        memcpy(ptr, info, table->item_size);
                        item = (aclk_hash_table_item_t *)(hdr->item);
                        item->addr = ptr;
                        hdr->used++;
                        hdr->next = cvmx_ptr_to_phys(hdr) >> ACLK_DPI_ADDR_SHIFT;
                        break;
                    }
                }
            }
        } else {
            ///malloc table item
            hdr = (aclk_hash_table_item_hdr_t *)cvmx_zone_alloc(table->table_zone, 0);
            if (NULL == hdr) {
                return -1;
            }
            memset(hdr, 0x00, table->table_size);
            ptr = cvmx_zone_alloc(table->item_zone, 0);
            if (NULL == ptr) {
                aclk_printf(ACLK_DPI_LOG_LEVEL_WARN, "%s[%d]:malloc for item zone error\n", __func__, __LINE__);
                return -1;
            }
            memcpy(ptr, info, table->item_size);
            item = (aclk_hash_table_item_t *)(hdr->item);
            item->addr = ptr;
            hdr->used++;
            hdr->next = cvmx_ptr_to_phys(hdr) >> ACLK_DPI_ADDR_SHIFT;
        }
    }
    aclk_hash_table_unlock(table, hash);
   
    return 0;
}

int aclk_hash_table_del(aclk_hash_table_info_t *table, uint32_t hash, void *info)
{
    unsigned int i;
    aclk_hash_table_item_hdr_t *hdr, *last, *cur;
    aclk_hash_table_item_t *item, *tmp;

    if (hash > table->table_num) {
        return -1;
    }

    aclk_hash_table_lock(table, hash);
    hdr = (aclk_hash_table_item_hdr_t *)(table->table + table->table_size * hash);
    for (i = 0; i < hdr->used; i++) {
        item = (aclk_hash_table_item_t *)(hdr->item + table->item_size * i);
        if (0 == table->compare(item->addr, info)) {
            cvmx_zone_free(table->item_zone, item->addr);
            item->addr = NULL;
            ///do del item
            if (0 == hdr->next) {
                tmp = (aclk_hash_table_item_t *)(hdr->item + table->item_size * (hdr->used - 1));
                item->addr = tmp->addr;
                tmp->addr = NULL;
                hdr->used--;
            } else {
                last = hdr;
                cur = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
                while (cur->next) {
                    last = cur;
                    cur = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(cur->next << ACLK_DPI_ADDR_SHIFT));
                }
                tmp = (aclk_hash_table_item_t *)(cur->item + table->item_size * (cur->used - 1));
                item->addr = tmp->addr;
                tmp->addr = NULL;
                if (1 == cur->used) {
                    cvmx_zone_free(table->table_zone, cur);
                    cur = NULL;
                    last->next = 0;
                } else {
                    cur->used--;
                }
                
            }

            aclk_hash_table_unlock(table, hash);
            return 0;
        }
    }

    if (hdr->next) {
        
        last = hdr;
        hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
        while (hdr) {
            ///storage addr->next 
            for (i = 0; i < hdr->used; i++) {
                item = (aclk_hash_table_item_t *)(hdr->item + table->item_size * i);
                if (0 == table->compare(item->addr, info)) {
                    cvmx_zone_free(table->item_zone, item->addr);
                    item->addr = NULL;
                    ///do del item
                    
                    if (0 == hdr->next) {
                        tmp = (aclk_hash_table_item_t *)(hdr->item + table->item_size * (hdr->used - 1));
                        item->addr = tmp->addr;
                        tmp->addr = NULL;
                        if (1 == hdr->used) {
                            cvmx_zone_free(table->table_zone, hdr);
                            hdr = NULL;
                            last->next = 0;
                        } else {
                            hdr->used--;
                        }
                    } else {
                        last = hdr;
                        cur = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
                        while (cur->next) {
                            last = cur;
                            cur = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(cur->next << ACLK_DPI_ADDR_SHIFT));
                        }
                        tmp = (aclk_hash_table_item_t *)(cur->item + table->item_size * (cur->used - 1));
                        item->addr = tmp->addr;
                        tmp->addr = NULL;
                        if (1 == cur->used) {
                            cvmx_zone_free(table->table_zone, cur);
                            cur = NULL;
                            last->next = 0;
                        } else {
                            cur->used--;
                        }
                    }

                    aclk_hash_table_unlock(table, hash);
                    return 0;
                }
            }
            
            if (hdr->next) {
                last = hdr;
                hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
            } else {
                break;
            }
        }
    }
    aclk_hash_table_unlock(table, hash);
   
    return 0;
}

int aclk_hash_table_timeout(aclk_hash_table_info_t *table, uint32_t hash)
{
    unsigned int i;
    aclk_hash_table_item_hdr_t *hdr, *last, *cur;
    aclk_hash_table_item_t *item, *tmp;

    if (hash > table->table_num) {
        return -1;
    }

    aclk_hash_table_lock(table, hash);
    hdr = (aclk_hash_table_item_hdr_t *)(table->table + table->table_size * hash);
    for (i = 0; i < hdr->used; i++) {
        item = (aclk_hash_table_item_t *)(hdr->item + table->item_size * i);
        if (table->timeout(item->addr)) {
            cvmx_zone_free(table->item_zone, item->addr);
            item->addr = NULL;
            ///do del item
            if (0 == hdr->next) {
                tmp = (aclk_hash_table_item_t *)(hdr->item + table->item_size * (hdr->used - 1));
                item->addr = tmp->addr;
                tmp->addr = NULL;
                hdr->used--;
            } else {
                last = hdr;
                cur = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
                while (cur->next) {
                    last = cur;
                    cur = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(cur->next << ACLK_DPI_ADDR_SHIFT));
                }
                tmp = (aclk_hash_table_item_t *)(cur->item + table->item_size * (cur->used - 1));
                item->addr = tmp->addr;
                tmp->addr = NULL;
                if (1 == cur->used) {
                    cvmx_zone_free(table->table_zone, cur);
                    cur = NULL;
                    last->next = 0;
                } else {
                    cur->used--;
                }
                
            }

            aclk_hash_table_unlock(table, hash);
            return 0;
        }
    }

    if (hdr->next) {
        
        last = hdr;
        hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
        while (hdr) {
            ///storage addr->next 
            for (i = 0; i < hdr->used; i++) {
                item = (aclk_hash_table_item_t *)(hdr->item + table->item_size * i);
                if (table->timeout(item->addr)) {
                    cvmx_zone_free(table->item_zone, item->addr);
                    item->addr = NULL;
                    ///do del item
                    
                    if (0 == hdr->next) {
                        tmp = (aclk_hash_table_item_t *)(hdr->item + table->item_size * (hdr->used - 1));
                        item->addr = tmp->addr;
                        tmp->addr = NULL;
                        if (1 == hdr->used) {
                            cvmx_zone_free(table->table_zone, hdr);
                            hdr = NULL;
                            last->next = 0;
                        } else {
                            hdr->used--;
                        }
                    } else {
                        last = hdr;
                        cur = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
                        while (cur->next) {
                            last = cur;
                            cur = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(cur->next << ACLK_DPI_ADDR_SHIFT));
                        }
                        tmp = (aclk_hash_table_item_t *)(cur->item + table->item_size * (cur->used - 1));
                        item->addr = tmp->addr;
                        tmp->addr = NULL;
                        if (1 == cur->used) {
                            cvmx_zone_free(table->table_zone, cur);
                            cur = NULL;
                            last->next = 0;
                        } else {
                            cur->used--;
                        }
                    }

                    aclk_hash_table_unlock(table, hash);
                    return 0;
                }
            }
            
            if (hdr->next) {
                last = hdr;
                hdr = (aclk_hash_table_item_hdr_t *)(cvmx_phys_to_ptr(hdr->next << ACLK_DPI_ADDR_SHIFT));
            } else {
                break;
            }
        }
    }
    aclk_hash_table_unlock(table, hash);
   
    return 0;
}

