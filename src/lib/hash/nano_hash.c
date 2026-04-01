#include "nano_hash.h"

void nano_hash( nano_hash_map_table_t* table, uint8_t* data , uint32_t data_len , uint8_t* hash , uint32_t hash_len )
{
    for (uint32_t i = 0; i < hash_len; i++)
    {
        uint32_t sum = 0;
        for (uint32_t j = i; j < data_len; j += hash_len)
        {
            sum += data[j];
        }
        hash[i] = table->table_arry[sum % table->table_size];
    }
}

