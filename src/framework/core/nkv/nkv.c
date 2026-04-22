#include "nkv.h"
#include <stddef.h>
#include <string.h>

#include "framework/nano_heap.h"
#define MALLOC(size)    nano_heap_malloc(size,NANO_HEAP_ATTR_DEFAULT)
#define FREE(ptr)       nano_heap_free(ptr)

#define DEBUG_LOG(...)
#define INFO_LOG(...)
#define WARN_LOG(...)
#define ERROR_LOG(...)

#if defined(__GNUC__) || defined(__clang__)
#define NKV_STACK_ALIGN4 __attribute__((aligned(4)))
#else
#define NKV_STACK_ALIGN4
#endif

static uint32_t nkv_cali_section_used(  nkv_ctrl_handle_t handle , section_meta_block_t* section_meta_block );
static void nkv_cali_kv_stats( nkv_ctrl_handle_t handle );
static uint32_t nkv_align_up_by_desc(nkv_ctrl_handle_t handle, uint32_t size);
static uint32_t nkv_section_meta_span_bytes(nkv_ctrl_handle_t handle);
static uint32_t nkv_kv_meta_span_bytes(nkv_ctrl_handle_t handle);
static uint32_t nkv_kv_key_offset_bytes(nkv_ctrl_handle_t handle);
static uint32_t nkv_kv_key_span_bytes(nkv_ctrl_handle_t handle, uint32_t key_size);
static uint32_t nkv_kv_value_offset_bytes(nkv_ctrl_handle_t handle, uint32_t key_size);
static uint32_t nkv_kv_value_span_bytes(nkv_ctrl_handle_t handle, uint32_t value_size);
static uint32_t nkv_kv_record_span_bytes(nkv_ctrl_handle_t handle, uint32_t key_size, uint32_t value_size);

/**
 * @brief CRC16计算函数，使用CCITT-FALSE算法，初始值为0x0000，生成多项式为0x1021
 * @param data 输入数据指针
 * @param len 输入数据长度，单位字节
 * @return CRC16校验值
*/
static uint16_t nkv_crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0x0000u;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= (uint16_t)(data[i] << 8);
        for (uint8_t bit = 0; bit < 8u; bit++) {
            if (crc & 0x8000u)  crc = (uint16_t)((crc << 1) ^ 0x8005u);
            else                crc <<= 1;
        }
    }
    return crc;
}

static uint32_t nkv_align_up_by_desc(nkv_ctrl_handle_t handle, uint32_t size)
{
    uint32_t align;
    if (handle == NULL || handle->desc.align_size == 0u) {
        return size;
    }
    align = handle->desc.align_size;
    return ((size + align - 1u) / align) * align;
}

static uint32_t nkv_section_meta_span_bytes(nkv_ctrl_handle_t handle)
{
    return nkv_align_up_by_desc(handle, (uint32_t)sizeof(section_meta_block_t));
}

static uint32_t nkv_kv_meta_span_bytes(nkv_ctrl_handle_t handle)
{
    return nkv_align_up_by_desc(handle, (uint32_t)sizeof(kv_meta_block_t));
}

static uint32_t nkv_kv_key_offset_bytes(nkv_ctrl_handle_t handle)
{
    return nkv_kv_meta_span_bytes(handle);
}

static uint32_t nkv_kv_key_span_bytes(nkv_ctrl_handle_t handle, uint32_t key_size)
{
    return nkv_align_up_by_desc(handle, key_size);
}

static uint32_t nkv_kv_value_offset_bytes(nkv_ctrl_handle_t handle, uint32_t key_size)
{
    return nkv_kv_key_offset_bytes(handle) + nkv_kv_key_span_bytes(handle, key_size);
}

static uint32_t nkv_kv_value_span_bytes(nkv_ctrl_handle_t handle, uint32_t value_size)
{
    return nkv_align_up_by_desc(handle, value_size);
}

static uint32_t nkv_kv_record_span_bytes(nkv_ctrl_handle_t handle, uint32_t key_size, uint32_t value_size)
{
    return nkv_kv_value_offset_bytes(handle, key_size) + nkv_kv_value_span_bytes(handle, value_size);
}

int nkv_ctrl_static_create(  nkv_ctrl_desc_t* desc, nkv_ctrl_t* ctrl )
{
    if( desc == NULL || desc->opt == NULL || desc->opt->write == NULL || desc->opt->read == NULL || desc->opt->erase == NULL )
    {
        ERROR_LOG("static create failed: invalid desc or callbacks");
        return -1;
    }

    //area大小的一半必须是擦除单元大小的整数倍
    if( desc->nkv_area_size == 0 || desc->erase_unit_size == 0 || desc->align_size == 0 ||
        (desc->nkv_area_size / 2) % desc->erase_unit_size != 0 )
    {
        ERROR_LOG("static create failed: area=%u erase_unit=%u align=%u",
                  (unsigned)desc->nkv_area_size,
                  (unsigned)desc->erase_unit_size,
                  (unsigned)desc->align_size);
        return -1;
    }

    if( ctrl == NULL )
    {
        ERROR_LOG("static create failed: malloc ctrl failed");
        return -1;
    }
    
    memset(ctrl,0,sizeof(nkv_ctrl_t));
    ctrl->desc = *desc;
    ctrl->section_size = ctrl->desc.nkv_area_size / 2;
    INFO_LOG("static create ok: area_start=%u area_size=%u section_size=%u",
             (unsigned)desc->nkv_area_start_addr,
             (unsigned)desc->nkv_area_size,
             (unsigned)ctrl->section_size);

    return 0;
}

nkv_ctrl_handle_t nkv_ctrl_create( nkv_ctrl_desc_t* desc )
{
    if( desc == NULL || desc->opt == NULL || desc->opt->write == NULL || desc->opt->read == NULL || desc->opt->erase == NULL )
    {
        ERROR_LOG("create failed: invalid desc or callbacks");
        return NULL;
    }

    //area大小的一半必须是擦除单元大小的整数倍
    if( desc->nkv_area_size == 0 || desc->erase_unit_size == 0 || desc->align_size == 0 ||
        (desc->nkv_area_size / 2) % desc->erase_unit_size != 0 )
    {
        ERROR_LOG("create failed: area=%u erase_unit=%u align=%u",
                  (unsigned)desc->nkv_area_size,
                  (unsigned)desc->erase_unit_size,
                  (unsigned)desc->align_size);
        return NULL;
    }

    nkv_ctrl_t* ctrl = (nkv_ctrl_t*)MALLOC(sizeof(nkv_ctrl_t));
    if( ctrl == NULL )
    {
        ERROR_LOG("create failed: malloc ctrl failed");
        return NULL;
    }
    
    memset(ctrl,0,sizeof(nkv_ctrl_t));
    ctrl->desc = *desc;
    ctrl->section_size = ctrl->desc.nkv_area_size / 2;
    INFO_LOG("create ok: area_start=%u area_size=%u section_size=%u",
             (unsigned)desc->nkv_area_start_addr,
             (unsigned)desc->nkv_area_size,
             (unsigned)ctrl->section_size);

    return (nkv_ctrl_handle_t)ctrl;
}

nkv_ctrl_handle_t nkv_ctrl_destroy( nkv_ctrl_handle_t handle )
{
    if( handle == NULL )
    {
        return NULL;
    }
    nkv_ctrl_t* ctrl = (nkv_ctrl_t*)handle;
    FREE(ctrl);
    return NULL;
}

/**
 * @brief 判断段是否合法
 * @param section_meta_block 段元数据块指针
 * @return 1表示合法，0表示不合法
*/
static uint8_t nkv_section_is_legal( section_meta_block_t* section_meta_block )
{
    if( section_meta_block == NULL )
    {
        return 0;
    }
    if( section_meta_block->type != META_BLOCK_TYPE_SECTION )
    {
        DEBUG_LOG("section illegal: type=0x%02X", (unsigned)section_meta_block->type);
        return 0;
    }
    //段信息的CRC16校验值计算
    uint16_t crc16 = nkv_crc16((uint8_t*)&section_meta_block->section_info,sizeof(section_info_t));
    if( crc16 != section_meta_block->info_crc16 )
    {
        DEBUG_LOG("section illegal: crc mismatch cali=0x%04X meta=0x%04X",
                  (unsigned)crc16, (unsigned)section_meta_block->info_crc16);
        return 0;
    }
    DEBUG_LOG("section legal: idx=%u start=%u size=%u",
              (unsigned)section_meta_block->section_info.section_index,
              (unsigned)section_meta_block->section_info.section_start_addr,
              (unsigned)section_meta_block->section_info.section_size);
    return 1;
}

/**
 * @brief 判断段是否有效
 * @param section_meta_block 段元数据块指针
 * @return 1表示有效，0表示无效
*/
static uint8_t nkv_section_is_active( section_meta_block_t* section_meta_block )
{
    if( section_meta_block == NULL )
    {
        return 0;
    }
    if( !nkv_section_is_legal(section_meta_block) )
    {
        return 0;
    }

    return section_meta_block->section_status.active;
}

static uint8_t nkv_section_is_stable_active(section_meta_block_t *section_meta_block)
{
    if (!nkv_section_is_active(section_meta_block)) {
        return 0u;
    }
    return (section_meta_block->section_status.sort == 0u) ? 1u : 0u;
}

static uint32_t nkv_section_abs_addr(nkv_ctrl_handle_t handle, uint32_t section_index)
{
    if (section_index == 0u) {
        return handle->desc.nkv_area_start_addr;
    }
    return handle->desc.nkv_area_start_addr + handle->section_size;
}

static uint32_t nkv_cali_section_used_by_meta(nkv_ctrl_handle_t handle, const section_meta_block_t *meta)
{
    section_meta_block_t bak NKV_STACK_ALIGN4;
    uint32_t used;

    if (handle == NULL || meta == NULL) {
        return 0u;
    }
    memcpy(&bak, &handle->active_section_meta_block, sizeof(bak));
    memcpy(&handle->active_section_meta_block, meta, sizeof(*meta));
    used = nkv_cali_section_used(handle, &handle->active_section_meta_block);
    memcpy(&handle->active_section_meta_block, &bak, sizeof(bak));
    return used;
}

/**
 * @brief 获取当前有效段
 * @param handle nkv控制器句柄
 * @param section_meta_block 段元数据块指针，输出参数，返回当前有效段的元数据块
 * @return 0:成功，-1表示读取失败，-2表示没有有效段
*/
static int nkv_get_active_section( nkv_ctrl_handle_t handle , section_meta_block_t* section_meta_block )
{
    section_meta_block_t sec0 NKV_STACK_ALIGN4;
    section_meta_block_t sec1 NKV_STACK_ALIGN4;
    uint8_t sec0_ok = 0u;
    uint8_t sec1_ok = 0u;

    memset(&sec0, 0, sizeof(sec0));
    memset(&sec1, 0, sizeof(sec1));

    //段0读取
    DEBUG_LOG("scan section0 meta at abs=%u", (unsigned)handle->desc.nkv_area_start_addr);
    if( handle->desc.opt->read(handle->desc.opt_ctx,handle->desc.nkv_area_start_addr,(uint8_t*)&sec0,sizeof(section_meta_block_t)) )
    {
        return -1;
    }
    sec0_ok = nkv_section_is_active(&sec0);

    //段1读取
    uint32_t section1_addr = handle->desc.nkv_area_start_addr + handle->section_size;
    DEBUG_LOG("scan section1 meta at abs=%u", (unsigned)section1_addr);
    if( handle->desc.opt->read(handle->desc.opt_ctx,section1_addr,(uint8_t*)&sec1,sizeof(section_meta_block_t)) )
    {
        return -1;
    }
    sec1_ok = nkv_section_is_active(&sec1);

    if (sec0_ok && sec1_ok) {
        uint8_t sec0_stable = nkv_section_is_stable_active(&sec0);
        uint8_t sec1_stable = nkv_section_is_stable_active(&sec1);

        if (sec0_stable && !sec1_stable) {
            memcpy(section_meta_block, &sec0, sizeof(section_meta_block_t));
            DEBUG_LOG("active section resolved: section0 (stable-sort)");
            return 0;
        }
        if (!sec0_stable && sec1_stable) {
            memcpy(section_meta_block, &sec1, sizeof(section_meta_block_t));
            DEBUG_LOG("active section resolved: section1 (stable-sort)");
            return 0;
        }

        /*
         * 两边都稳定或都在 sort 状态时，按 used 决策。
         */
        {
            uint32_t used0 = nkv_cali_section_used_by_meta(handle, &sec0);
            uint32_t used1 = nkv_cali_section_used_by_meta(handle, &sec1);
            if (used1 > used0) {
                memcpy(section_meta_block, &sec1, sizeof(section_meta_block_t));
                DEBUG_LOG("active section resolved: section1 (dual-active, used1=%u used0=%u)",
                          (unsigned)used1, (unsigned)used0);
            } else {
                memcpy(section_meta_block, &sec0, sizeof(section_meta_block_t));
                DEBUG_LOG("active section resolved: section0 (dual-active, used0=%u used1=%u)",
                          (unsigned)used0, (unsigned)used1);
            }
        }
        return 0;
    }
    if (sec0_ok) {
        memcpy(section_meta_block, &sec0, sizeof(section_meta_block_t));
        DEBUG_LOG("active section found: section0");
        return 0;
    }
    if (sec1_ok) {
        memcpy(section_meta_block, &sec1, sizeof(section_meta_block_t));
        DEBUG_LOG("active section found: section1");
        return 0;
    }

    DEBUG_LOG("no active section in section0/section1");
    return -2;
}

/**
 * @brief 读取键值对元数据块
 * @param handle nkv控制器句柄
 * @param addr 键值对元数据块地址，单位字节，必须是有效段内的地址（area内的相对地址）
 * @param kv_meta_block 键值对元数据块指针，输出参数，返回读取到的键值对元数据块
 * @return 0:成功，-1表示读取失败，-2表示输入参数不合法，-3表示数据校验失败
*/
static int nkv_read_kv_meta_block( nkv_ctrl_handle_t handle , uint32_t addr , kv_meta_block_t* kv_meta_block )
{
    if( handle == NULL || kv_meta_block == NULL )
    {
        return -2;
    }

    if( addr < handle->active_section_meta_block.section_info.section_start_addr || 
        addr >= handle->active_section_meta_block.section_info.section_start_addr + handle->active_section_meta_block.section_info.section_size )
    {
        DEBUG_LOG("read kv meta out of range: addr=%u section=[%u,%u)",
                  (unsigned)addr,
                  (unsigned)handle->active_section_meta_block.section_info.section_start_addr,
                  (unsigned)(handle->active_section_meta_block.section_info.section_start_addr +
                             handle->active_section_meta_block.section_info.section_size));
        return -2;
    }

    memset(kv_meta_block,0,sizeof(kv_meta_block_t));
    if( handle->desc.opt->read(handle->desc.opt_ctx,addr,(uint8_t*)kv_meta_block,sizeof(kv_meta_block_t)) )
    {
        DEBUG_LOG("read kv meta failed at addr=%u", (unsigned)addr);
        return -1;
    }

    if( kv_meta_block->type != META_BLOCK_TYPE_KV )
    {
        DEBUG_LOG("read kv meta invalid type addr=%u type=0x%02X", (unsigned)addr, (unsigned)kv_meta_block->type);
        return -2;
    }

    //计算info的CRC16校验值
    uint16_t crc16 = nkv_crc16((uint8_t*)&kv_meta_block->kv_info,sizeof(kv_info_t));
    if( crc16 != kv_meta_block->info_crc16 )
    {
        DEBUG_LOG("read kv meta crc mismatch addr=%u cali=0x%04X meta=0x%04X",
                  (unsigned)addr, (unsigned)crc16, (unsigned)kv_meta_block->info_crc16);
        return -3;
    }
    DEBUG_LOG("read kv meta ok addr=%u key_size=%u value_size=%u active=%u next_off=%u",
              (unsigned)addr,
              (unsigned)kv_meta_block->kv_info.key_size,
              (unsigned)kv_meta_block->kv_info.value_size,
              (unsigned)kv_meta_block->kv_status.active,
              (unsigned)kv_meta_block->kv_status.next_same_meta_block_offset);

    return 0;
}

/**
 * @brief 计算段已使用空间大小
 * @param handle nkv控制器句柄
 * @param section_meta_block 段元数据块指针，输入参数，段元数据块必须合法且有效
 * @return 段已使用空间大小
*/
static uint32_t nkv_cali_section_used(  nkv_ctrl_handle_t handle , section_meta_block_t* section_meta_block )
{
    if( handle == NULL || section_meta_block == NULL )
    {
        return -1;
    }
    if( !nkv_section_is_active(section_meta_block) )
    {
        return -1;
    }

    uint32_t used = 0;
    kv_meta_block_t kv_meta_block NKV_STACK_ALIGN4;
    used += nkv_section_meta_span_bytes(handle); //段元数据块占用空间(按配置对齐)
    DEBUG_LOG("calc section used start=%u section_size=%u",
              (unsigned)used,
              (unsigned)section_meta_block->section_info.section_size);
    while(1)
    {
        if( nkv_read_kv_meta_block( handle , section_meta_block->section_info.section_start_addr + used , &kv_meta_block ) )
        {
            DEBUG_LOG("calc section used stop at used=%u", (unsigned)used);
            return used;
        }

        used += nkv_kv_record_span_bytes(handle,
                                         (uint32_t)kv_meta_block.kv_info.key_size,
                                         (uint32_t)kv_meta_block.kv_info.value_size);
        DEBUG_LOG("calc section used advance=%u", (unsigned)used);
    }

    return section_meta_block->section_info.section_size;
}

/**
 * @brief 比较键值对元数据块中的键与输入的键是否相同
 * @param handle nkv控制器句柄
 * @param kv_meta_block 键值对元数据块指针，输入参数，必须合法且有效
 * @param meta_block_addr 键值对元数据块地址，单位字节，输入参数，必须是有效段内的地址（area内的相对地址）
 * @param key 键字符串，输入参数
 * @return 0表示相同，-1表示不同，-2表示输入参数不合法
*/
static int nkv_key_compare( nkv_ctrl_handle_t handle , kv_meta_block_t* kv_meta_block , uint32_t meta_block_addr , const char* key )
{
    if( handle == NULL || kv_meta_block == NULL || key == NULL )
    {
        return -2;
    }

    if( kv_meta_block->kv_info.key_size != (strlen(key) + 1u) )
    {
        DEBUG_LOG("key compare size mismatch: meta=%u input=%u key=%s",
                  (unsigned)kv_meta_block->kv_info.key_size,
                  (unsigned)(strlen(key) + 1u),
                  key);
        return -1;
    }

    uint8_t key_read_buf[256];
    uint32_t key_read_addr = meta_block_addr + nkv_kv_key_offset_bytes(handle);
    if( handle->desc.opt->read(handle->desc.opt_ctx,key_read_addr,key_read_buf,kv_meta_block->kv_info.key_size) )
    {
        return -1;
    }

    uint16_t key_crc16 = nkv_crc16(key_read_buf,kv_meta_block->kv_info.key_size);
    if( key_crc16 != kv_meta_block->kv_info.key_crc16 )
    {
        return -1;
    }

    if( strcmp((char*)key_read_buf,key) != 0 )
    {
        return -1;
    }

    return 0;
}

/**
 * @brief 查找最后一个同名键值对元数据块
 * @param handle nkv控制器句柄
 * @param key 键字符串
 * @param kv_meta_block 键值对元数据块指针，输出参数，返回找到的键值对元数据块
 * @param kv_meta_block_addr 键值对元数据块地址，输出参数，返回找到的键值对元数据块地址，
 * @return 0:成功，-1表示读取失败，-2表示输入参数不合法，-3表示数据校验失败，-4表示没有找到同名键值对
*/
static int nkv_find_last_kv_meta_block( nkv_ctrl_handle_t handle , const char* key , kv_meta_block_t* kv_meta_block , uint32_t* kv_meta_block_addr )
{
    if(  handle == NULL || key == NULL || kv_meta_block == NULL || kv_meta_block_addr == NULL )
    {
        return -2;
    }

    uint32_t addr = handle->desc.nkv_area_start_addr + handle->active_section_meta_block.section_info.section_start_addr + nkv_section_meta_span_bytes(handle);
    uint32_t end_addr = handle->desc.nkv_area_start_addr + handle->active_section_meta_block.section_info.section_start_addr + handle->active_section_meta_block.section_info.section_size;
    DEBUG_LOG("find last kv start key=%s addr=%u end=%u", key, (unsigned)addr, (unsigned)end_addr);

    //先找到首个同名块
    while( addr < end_addr )
    {
        if( nkv_read_kv_meta_block( handle , addr , kv_meta_block ) )
        {
            DEBUG_LOG("find first same key stop read fail addr=%u", (unsigned)addr);
            return -1;
        }

        if( nkv_key_compare( handle , kv_meta_block , addr , key ) == 0 )
        {
            DEBUG_LOG("find first same key hit addr=%u", (unsigned)addr);
            break;
        }

        addr += nkv_kv_record_span_bytes(handle,
                                         (uint32_t)kv_meta_block->kv_info.key_size,
                                         (uint32_t)kv_meta_block->kv_info.value_size);
        DEBUG_LOG("find first same key next addr=%u", (unsigned)addr);
    }

    if( addr >= end_addr )
    {
        DEBUG_LOG("find key not found: key=%s", key);
        return -4;
    }

    //沿 next_same_meta_block_offset 链索引到最后一个同名块
    while( addr < end_addr )
    {
        if( kv_meta_block->kv_status.next_same_meta_block_offset == 0xFFFFFFFF )
        {
            DEBUG_LOG("find last key tail addr=%u", (unsigned)addr);
            break;
        }

        addr += kv_meta_block->kv_status.next_same_meta_block_offset;
        DEBUG_LOG("find last key jump addr=%u", (unsigned)addr);

        if( nkv_read_kv_meta_block( handle , addr , kv_meta_block ) )
        {
            DEBUG_LOG("find last key jump read fail addr=%u", (unsigned)addr);
            return -1;
        }
    }

    *kv_meta_block_addr = addr;
    DEBUG_LOG("find last kv done key=%s addr=%u", key, (unsigned)addr);
    return 0;
}

/**
 * @brief 扫描当前有效段，统计活跃链尾键值对数量及单条记录总占用（meta+key+value）
 */
static void nkv_cali_kv_stats( nkv_ctrl_handle_t handle )
{
    uint32_t used;
    kv_meta_block_t kv NKV_STACK_ALIGN4;
    uint32_t cnt = 0u;
    uint32_t total = 0u;
    uint32_t rel_addr;
    uint32_t step;

    if (handle == NULL || handle->is_init == 0u) {
        return;
    }

    used = nkv_section_meta_span_bytes(handle);
    while (1) {
        rel_addr = handle->active_section_meta_block.section_info.section_start_addr + used;
        if (nkv_read_kv_meta_block(handle, rel_addr, &kv)) {
            break;
        }
        step = nkv_kv_record_span_bytes(handle, (uint32_t)kv.kv_info.key_size, (uint32_t)kv.kv_info.value_size);
        if (step == 0u) {
            break;
        }
        if (kv.kv_status.active != 0u && kv.kv_status.next_same_meta_block_offset == 0xFFFFFFFFu) {
            cnt++;
            total += step;
        }
        used += step;
    }

    handle->kv_cnt = cnt;
    handle->kv_total_size = total;
}

int nkv_init( nkv_ctrl_handle_t handle )
{
    uint32_t section_left;

    if( handle == NULL )
    {
        ERROR_LOG("init failed: handle is null");
        return -1;
    }
    
    if( nkv_get_active_section(handle, &handle->active_section_meta_block) )
    {
        ERROR_LOG("init failed: no active section");
        return -1;
    }

    if( handle->section_size != handle->active_section_meta_block.section_info.section_size )
    {
        ERROR_LOG("init failed: section_size mismatch cfg=%u meta=%u",
                  (unsigned)handle->section_size,
                  (unsigned)handle->active_section_meta_block.section_info.section_size);
        return -1;
    }

    handle->section_used = nkv_cali_section_used(handle, &handle->active_section_meta_block);
    nkv_cali_kv_stats(handle);
    handle->is_init = 1;
    INFO_LOG("init ok: section_idx=%u section_used=%u kv_cnt=%u kv_total_size=%u",
             (unsigned)handle->active_section_meta_block.section_info.section_index,
             (unsigned)handle->section_used,
             (unsigned)handle->kv_cnt,
             (unsigned)handle->kv_total_size);

    if (handle->section_used >= handle->section_size) {
        section_left = 0u;
    } else {
        section_left = handle->section_size - handle->section_used;
    }
    if (section_left < 1024u) {
        WARN_LOG("init pre-sort: section_left=%u (<1024), try sort", (unsigned)section_left);
        if (nkv_ctrl_sort(handle) != 0) {
            WARN_LOG("init pre-sort failed");
        }
    }

    return 0;
}

int nkv_ctrl_reset( nkv_ctrl_handle_t handle )
{
    if( handle == NULL )
    {
        ERROR_LOG("reset failed: handle is null");
        return -1;
    }

    //全域擦除
    if( handle->desc.opt->erase(handle->desc.opt_ctx,handle->desc.nkv_area_start_addr,handle->desc.nkv_area_size) )
    {
        ERROR_LOG("reset failed: erase area failed");
        return -1;
    }

    //构造段0
    section_meta_block_t section_meta_block NKV_STACK_ALIGN4;
    memset(&section_meta_block,0,sizeof(section_meta_block_t));
    section_meta_block.type = META_BLOCK_TYPE_SECTION;
    section_meta_block.section_info.area_size = handle->desc.nkv_area_size;
    section_meta_block.section_info.section_index = 0;
    section_meta_block.section_info.section_size = handle->section_size;
    section_meta_block.section_info.section_start_addr = 0;
    section_meta_block.section_info.version = 1;
    section_meta_block.info_crc16 = nkv_crc16((uint8_t*)&section_meta_block.section_info,sizeof(section_info_t));
    section_meta_block.section_status.active = 1;
    section_meta_block.section_status.sort = 0;
    section_meta_block.section_status.reserved = 0;

    //写入段0
    if( handle->desc.opt->write(handle->desc.opt_ctx,handle->desc.nkv_area_start_addr,(uint8_t*)&section_meta_block,sizeof(section_meta_block_t)) )
    {
        ERROR_LOG("reset failed: write section meta failed");
        return -1;
    }

    //同步到段ram
    memcpy(&handle->active_section_meta_block,&section_meta_block,sizeof(section_meta_block_t));
    handle->section_used = nkv_section_meta_span_bytes(handle);
    handle->kv_cnt = 0u;
    handle->kv_total_size = 0u;
    handle->is_init = 1;
    INFO_LOG("reset ok: section_used=%u", (unsigned)handle->section_used);

    return 0;
}

/**
 * @brief 构造键值对元数据块
 * @param kv_meta_block 键值对元数据块指针，输出参数，返回构造好的键值对元数据块
 * @param key 键字符串，输入参数
 * @param value 值数据指针，输入参数
 * @param size 值数据大小，单位字节，输入参数
*/
static void nkv_make_kv_meta_block( kv_meta_block_t* kv_meta_block , const char* key , const void* value , uint32_t size )
{
    //构造新的键值对元数据块
    kv_meta_block->type = META_BLOCK_TYPE_KV;
    kv_meta_block->kv_info.key_size = strlen(key) + 1;
    kv_meta_block->kv_info.value_size = size;
    kv_meta_block->kv_info.key_crc16 = nkv_crc16((uint8_t*)key,kv_meta_block->kv_info.key_size);
    kv_meta_block->kv_info.value_crc16 = nkv_crc16((uint8_t*)value,kv_meta_block->kv_info.value_size);
    kv_meta_block->info_crc16 = nkv_crc16((uint8_t*)&kv_meta_block->kv_info,sizeof(kv_info_t));
    kv_meta_block->kv_status.active = 1;
    kv_meta_block->kv_status.next_same_meta_block_offset = 0xFFFFFFFF; //默认没有下一个同名块
    kv_meta_block->kv_status.reserved = 0;
}

/**
 * @brief 设置旧的键值对元数据块无效，并更新next_same_meta_block_offset字段指向新的键值对元数据块
 * @param handle nkv控制器句柄
 * @param old_kv_meta_block_addr 旧的键值对元数据块地址，单位字节，必须是有效段内的地址（area内的相对地址）
 * @param kv_meta_next_addr 新的键值对元数据块地址，单位字节，必须是有效段内的地址（area内的相对地址）
 * @return 0:成功，-1表示写入失败，-2表示输入参数不合法
*/
static void nkv_set_old_kv_meta_block_valid( nkv_ctrl_handle_t handle , uint32_t old_kv_meta_block_addr , uint32_t kv_meta_next_addr )
{
    if( handle == NULL )
    {
        return;
    }

    kv_meta_block_t old_kv_meta_block NKV_STACK_ALIGN4;
    if( nkv_read_kv_meta_block( handle , old_kv_meta_block_addr , &old_kv_meta_block ) )
    {
        return;
    }

    old_kv_meta_block.kv_status.active = 0;
    old_kv_meta_block.kv_status.next_same_meta_block_offset = kv_meta_next_addr - old_kv_meta_block_addr;

    handle->desc.opt->write(handle->desc.opt_ctx,old_kv_meta_block_addr,(uint8_t*)&old_kv_meta_block,sizeof(kv_meta_block_t));
}

static int nkv_read_bytes_abs(nkv_ctrl_handle_t handle, uint32_t abs_addr, uint8_t *buf, uint32_t size)
{
    if (handle == NULL || buf == NULL || size == 0u) {
        return -1;
    }
    if (handle->desc.opt == NULL || handle->desc.opt->read == NULL) {
        return -1;
    }
    if (handle->desc.opt->read(handle->desc.opt_ctx, abs_addr, buf, size)) {
        return -1;
    }
    return 0;
}

static int nkv_write_bytes_abs(nkv_ctrl_handle_t handle, uint32_t abs_addr, const uint8_t *buf, uint32_t size)
{
    if (handle == NULL || buf == NULL || size == 0u) {
        return -1;
    }
    if (handle->desc.opt == NULL || handle->desc.opt->write == NULL) {
        return -1;
    }
    if (handle->desc.opt->write(handle->desc.opt_ctx, abs_addr, buf, size)) {
        return -1;
    }
    return 0;
}

static int nkv_sort_copy_one_record(
    nkv_ctrl_handle_t handle,
    uint32_t src_kv_abs_addr,
    kv_meta_block_t *src_kv,
    uint32_t dst_section_abs_addr,
    uint32_t *dst_used)
{
    uint8_t key_buf[128];
    uint8_t *val_buf = NULL;
    uint32_t dst_abs_addr;
    uint32_t record_size;

    if (handle == NULL || src_kv == NULL || dst_used == NULL) {
        return -1;
    }
    if (src_kv->kv_info.key_size == 0u || src_kv->kv_info.key_size > (uint8_t)sizeof(key_buf)) {
        return -1;
    }

    record_size = nkv_kv_record_span_bytes(handle, (uint32_t)src_kv->kv_info.key_size, (uint32_t)src_kv->kv_info.value_size);
    if ((*dst_used + record_size) > handle->section_size) {
        return -2;
    }

    if (nkv_read_bytes_abs(handle,
                           src_kv_abs_addr + nkv_kv_key_offset_bytes(handle),
                           key_buf,
                           src_kv->kv_info.key_size)) {
        return -1;
    }

    if (src_kv->kv_info.value_size > 0u) {
        val_buf = (uint8_t *)MALLOC(src_kv->kv_info.value_size);
        if (val_buf == NULL) {
            return -1;
        }
        if (nkv_read_bytes_abs(handle,
                               src_kv_abs_addr + nkv_kv_value_offset_bytes(handle, (uint32_t)src_kv->kv_info.key_size),
                               val_buf,
                               src_kv->kv_info.value_size)) {
            FREE(val_buf);
            return -1;
        }
    }

    dst_abs_addr = dst_section_abs_addr + *dst_used;
    if (nkv_write_bytes_abs(handle, dst_abs_addr, (const uint8_t *)src_kv, sizeof(kv_meta_block_t))) {
        if (val_buf != NULL) {
            FREE(val_buf);
        }
        return -1;
    }
    dst_abs_addr += nkv_kv_key_offset_bytes(handle);
    if (nkv_write_bytes_abs(handle, dst_abs_addr, key_buf, src_kv->kv_info.key_size)) {
        if (val_buf != NULL) {
            FREE(val_buf);
        }
        return -1;
    }
    dst_abs_addr += nkv_kv_key_span_bytes(handle, (uint32_t)src_kv->kv_info.key_size);
    if (src_kv->kv_info.value_size > 0u) {
        if (nkv_write_bytes_abs(handle, dst_abs_addr, val_buf, src_kv->kv_info.value_size)) {
            FREE(val_buf);
            return -1;
        }
        FREE(val_buf);
    }

    *dst_used += record_size;
    return 0;
}

int nkv_ctrl_sort( nkv_ctrl_handle_t handle )
{
    uint32_t old_idx;
    uint32_t new_idx;
    uint32_t old_rel_start;
    uint32_t old_abs_start;
    uint32_t old_abs_end;
    uint32_t new_abs_start;
    uint32_t scan_abs;
    uint32_t dst_used;
    section_meta_block_t old_meta NKV_STACK_ALIGN4;
    section_meta_block_t new_meta NKV_STACK_ALIGN4;
    kv_meta_block_t kv NKV_STACK_ALIGN4;

    if (handle == NULL) {
        return -1;
    }
    if (handle->is_init == 0u) {
        return -1;
    }
    if (handle->desc.opt == NULL ||
        handle->desc.opt->read == NULL || handle->desc.opt->write == NULL || handle->desc.opt->erase == NULL) {
        return -1;
    }

    old_idx = (uint32_t)handle->active_section_meta_block.section_info.section_index;
    if (old_idx > 1u) {
        old_idx = (handle->active_section_meta_block.section_info.section_start_addr == 0u) ? 0u : 1u;
    }
    new_idx = (old_idx == 0u) ? 1u : 0u;

    old_rel_start = handle->active_section_meta_block.section_info.section_start_addr;
    old_abs_start = handle->desc.nkv_area_start_addr + old_rel_start;
    old_abs_end = old_abs_start + handle->section_used;
    new_abs_start = nkv_section_abs_addr(handle, new_idx);

    memset(&old_meta, 0, sizeof(old_meta));
    if (nkv_read_bytes_abs(handle, old_abs_start, (uint8_t *)&old_meta, sizeof(old_meta))) {
        ERROR_LOG("sort failed: read old section meta");
        return -2;
    }
    if (!nkv_section_is_legal(&old_meta)) {
        ERROR_LOG("sort failed: old section meta illegal");
        return -2;
    }

    if (handle->desc.opt->erase(handle->desc.opt_ctx, new_abs_start, handle->section_size)) {
        ERROR_LOG("sort failed: erase target section");
        return -3;
    }

    memset(&new_meta, 0, sizeof(new_meta));
    new_meta.type = META_BLOCK_TYPE_SECTION;
    new_meta.section_info.area_size = handle->desc.nkv_area_size;
    new_meta.section_info.section_index = (uint16_t)new_idx;
    new_meta.section_info.section_size = handle->section_size;
    new_meta.section_info.section_start_addr = (new_idx == 0u) ? 0u : handle->section_size;
    new_meta.section_info.version = old_meta.section_info.version;
    new_meta.section_info.reserved = 0u;
    memset(new_meta.section_info.info_recv, 0, sizeof(new_meta.section_info.info_recv));
    new_meta.info_crc16 = nkv_crc16((uint8_t *)&new_meta.section_info, sizeof(section_info_t));
    /* 复制阶段先保持 inactive，掉电时可稳定回退旧分区。 */
    new_meta.section_status.active = 1u;
    new_meta.section_status.sort = 1u;
    new_meta.section_status.reserved = 0u;

    if (nkv_write_bytes_abs(handle, new_abs_start, (const uint8_t *)&new_meta, sizeof(new_meta))) {
        ERROR_LOG("sort failed: write target section meta");
        return -3;
    }

    dst_used = nkv_section_meta_span_bytes(handle);
    scan_abs = old_abs_start + nkv_section_meta_span_bytes(handle);

    while (scan_abs + sizeof(kv_meta_block_t) <= old_abs_end) {
        uint32_t src_rel_addr;
        uint32_t step;

        memset(&kv, 0, sizeof(kv));
        if (nkv_read_bytes_abs(handle, scan_abs, (uint8_t *)&kv, sizeof(kv))) {
            break;
        }
        if (kv.type != META_BLOCK_TYPE_KV) {
            break;
        }
        {
            uint16_t info_crc = nkv_crc16((uint8_t *)&kv.kv_info, sizeof(kv_info_t));
            if (info_crc != kv.info_crc16) {
                break;
            }
        }

        step = nkv_kv_record_span_bytes(handle, (uint32_t)kv.kv_info.key_size, (uint32_t)kv.kv_info.value_size);
        if (step == 0u) {
            break;
        }

        if (kv.kv_status.active && kv.kv_status.next_same_meta_block_offset == 0xFFFFFFFFu) {
            src_rel_addr = scan_abs - handle->desc.nkv_area_start_addr;
            (void)src_rel_addr;
            if (nkv_sort_copy_one_record(handle, scan_abs, &kv, new_abs_start, &dst_used)) {
                ERROR_LOG("sort failed: copy kv record addr=%u", (unsigned)src_rel_addr);
                return -4;
            }
        }

        scan_abs += step;
    }

    /* 复制完成后清 sort 标志（1->0），目标段转为稳定可用。 */
    new_meta.section_status.sort = 0u;
    if (nkv_write_bytes_abs(handle, new_abs_start, (const uint8_t *)&new_meta, sizeof(new_meta))) {
        ERROR_LOG("sort failed: activate new section meta");
        return -5;
    }

    memcpy(&handle->active_section_meta_block, &new_meta, sizeof(new_meta));
    handle->section_used = dst_used;
    nkv_cali_kv_stats(handle);
    handle->is_init = 1u;
    INFO_LOG("sort ok: old=%u new=%u used=%u kv_cnt=%u kv_total_size=%u",
             (unsigned)old_idx,
             (unsigned)new_idx,
             (unsigned)dst_used,
             (unsigned)handle->kv_cnt,
             (unsigned)handle->kv_total_size);
    return 0;
}

int nkv_ctrl_set_value( nkv_ctrl_handle_t handle , const char* key , const void* value , uint32_t size )
{
    uint32_t need_bytes;
    uint32_t expect_used_after_set;
    uint32_t left_after_set;
    int sort_ret;

    if( handle == NULL || key == NULL || value == NULL || size == 0 )
    {
        ERROR_LOG("set failed: invalid args");
        return -1;
    }
    if( handle->is_init == 0 )
    {
        ERROR_LOG("set failed: ctrl not initialized");
        return -1;
    }
    
    kv_meta_block_t kv_block NKV_STACK_ALIGN4;
    kv_meta_block_t last_kv_block NKV_STACK_ALIGN4;
    uint32_t last_kv_block_addr;
    DEBUG_LOG("set begin key=%s size=%u section_used=%u", key, (unsigned)size, (unsigned)handle->section_used);

    need_bytes = nkv_kv_record_span_bytes(handle, (uint32_t)strlen(key) + 1u, size);
    expect_used_after_set = handle->section_used + need_bytes;
    if (expect_used_after_set >= handle->section_size) {
        left_after_set = 0u;
    } else {
        left_after_set = handle->section_size - expect_used_after_set;
    }
    if (left_after_set < 1024u) {
        WARN_LOG("set pre-sort: key=%s need=%u left_after_set=%u section_used=%u section_size=%u",
                 key,
                 (unsigned)need_bytes,
                 (unsigned)left_after_set,
                 (unsigned)handle->section_used,
                 (unsigned)handle->section_size);
        sort_ret = nkv_ctrl_sort(handle);
        if (sort_ret != 0) {
            ERROR_LOG("set failed: pre-sort failed ret=%d key=%s", sort_ret, key);
            return -1;
        }
        expect_used_after_set = handle->section_used + need_bytes;
        if (expect_used_after_set > handle->section_size) {
            ERROR_LOG("set failed: no space after sort key=%s need=%u used=%u size=%u",
                      key,
                      (unsigned)need_bytes,
                      (unsigned)handle->section_used,
                      (unsigned)handle->section_size);
            return -1;
        }
    }

    //构造新的键值对元数据块
    nkv_make_kv_meta_block(&kv_block,key,value,size);

    //查找最后一个同名块
    if( !nkv_find_last_kv_meta_block(handle, key , &last_kv_block , &last_kv_block_addr) )
    {
        //存在同名块，更新原块的active字段和next_same_meta_block_offset字段
        DEBUG_LOG("set found old key=%s old_addr=%u", key, (unsigned)last_kv_block_addr);
        nkv_set_old_kv_meta_block_valid(handle, last_kv_block_addr , handle->active_section_meta_block.section_info.section_start_addr + handle->section_used );
    }

    //写入新的键值对元数据块
    uint32_t abs_addr = handle->desc.nkv_area_start_addr;
    abs_addr += handle->active_section_meta_block.section_info.section_start_addr;
    abs_addr += handle->section_used;
    DEBUG_LOG("set write kv meta abs_addr=%u", (unsigned)abs_addr);
    if( handle->desc.opt->write(handle->desc.opt_ctx,abs_addr,(uint8_t*)&kv_block,sizeof(kv_meta_block_t)) )
    {
        ERROR_LOG("set failed: write kv meta key=%s", key);
        return -1;
    }

    //写入键
    abs_addr += nkv_kv_key_offset_bytes(handle);
    DEBUG_LOG("set write key abs_addr=%u key_size=%u", (unsigned)abs_addr, (unsigned)kv_block.kv_info.key_size);
    if( handle->desc.opt->write(handle->desc.opt_ctx,abs_addr,(uint8_t*)key,kv_block.kv_info.key_size) )
    {
        ERROR_LOG("set failed: write key data key=%s", key);
        return -1;
    }

    //写入值
    abs_addr += nkv_kv_key_span_bytes(handle, (uint32_t)kv_block.kv_info.key_size);
    DEBUG_LOG("set write value abs_addr=%u value_size=%u", (unsigned)abs_addr, (unsigned)kv_block.kv_info.value_size);
    if( handle->desc.opt->write(handle->desc.opt_ctx,abs_addr,(uint8_t*)value,kv_block.kv_info.value_size) )
    {
        ERROR_LOG("set failed: write value key=%s size=%u", key, (unsigned)size);
        return -1;
    }
    handle->section_used += nkv_kv_record_span_bytes(handle,
                                                     (uint32_t)kv_block.kv_info.key_size,
                                                     (uint32_t)kv_block.kv_info.value_size);
    INFO_LOG("set ok: key=%s size=%u section_used=%u", key, (unsigned)size, (unsigned)handle->section_used);
    if( handle->section_used < handle->section_size )
    {
        uint32_t section_left = handle->section_size - handle->section_used;
        if( section_left < 512u )
        {
            WARN_LOG("set warn: section left=%u bytes (<512), key=%s section_used=%u section_size=%u",
                     (unsigned)section_left,
                     key,
                     (unsigned)handle->section_used,
                     (unsigned)handle->section_size);
        }
    }
    else
    {
        WARN_LOG("set warn: section full/overflow, section_used=%u section_size=%u key=%s",
                 (unsigned)handle->section_used,
                 (unsigned)handle->section_size,
                 key);
    }

    return 0;
}

int nkv_ctrl_get_value( nkv_ctrl_handle_t handle , const char* key , void* value , uint32_t* size )
{
    DEBUG_LOG("get begin key=%s req_size=%u", key, (unsigned)*size);
    if( handle == NULL || key == NULL || value == NULL || size == NULL || *size == 0 )
    {
        ERROR_LOG("get failed: invalid args");
        return -1;
    }
    if( handle->is_init == 0 )
    {
        ERROR_LOG("get failed: ctrl not initialized");
        return -1;
    }

    kv_meta_block_t kv_meta_block NKV_STACK_ALIGN4;
    uint32_t kv_meta_block_addr;

    //查找最后一个同名块
    if( nkv_find_last_kv_meta_block(handle, key , &kv_meta_block , &kv_meta_block_addr) )
    {
        WARN_LOG("get miss: key=%s", key);
        return -1;
    }

    //判断键值对是否有效
    if( kv_meta_block.kv_status.active == 0 )
    {
        WARN_LOG("get inactive key: key=%s", key);
        return -1;
    }

    //根据输入的size参数和实际值大小，确定需要读取的值大小
    uint32_t need_read_value_size = *size > kv_meta_block.kv_info.value_size ? kv_meta_block.kv_info.value_size : *size;
    DEBUG_LOG("get value info key=%s kv_addr=%u kv_value_size=%u read_size=%u",
              key,
              (unsigned)kv_meta_block_addr,
              (unsigned)kv_meta_block.kv_info.value_size,
              (unsigned)need_read_value_size);

    //读取值
    uint32_t value_addr = handle->desc.nkv_area_start_addr +
                          kv_meta_block_addr +
                          nkv_kv_value_offset_bytes(handle, (uint32_t)kv_meta_block.kv_info.key_size);
    DEBUG_LOG("get read value abs_addr=%u", (unsigned)value_addr);
    if( handle->desc.opt->read(handle->desc.opt_ctx,value_addr,(uint8_t*)value,need_read_value_size) )
    {
        ERROR_LOG("get failed: read value key=%s", key);
        return -1;
    }

    *size = need_read_value_size;
    INFO_LOG("get ok: key=%s size=%u", key, (unsigned)need_read_value_size);
    return 0;
}

uint32_t nkv_ctrl_get_value_size( nkv_ctrl_handle_t handle , const char* key )
{
    kv_meta_block_t kv_meta_block NKV_STACK_ALIGN4;
    uint32_t kv_meta_block_addr;

    if( handle == NULL || key == NULL )
    {
        ERROR_LOG("get size failed: invalid args");
        return 0u;
    }
    if( handle->is_init == 0 )
    {
        ERROR_LOG("get size failed: ctrl not initialized");
        return 0u;
    }

    if( nkv_find_last_kv_meta_block(handle, key, &kv_meta_block, &kv_meta_block_addr) )
    {
        DEBUG_LOG("get size miss: key=%s", key);
        return 0u;
    }
    if( kv_meta_block.kv_status.active == 0 )
    {
        DEBUG_LOG("get size inactive: key=%s addr=%u", key, (unsigned)kv_meta_block_addr);
        return 0u;
    }

    DEBUG_LOG("get size ok: key=%s size=%u addr=%u",
              key,
              (unsigned)kv_meta_block.kv_info.value_size,
              (unsigned)kv_meta_block_addr);
    return kv_meta_block.kv_info.value_size;
}

int nkv_ctrl_remove_kv( nkv_ctrl_handle_t handle , const char* key )
{
    kv_meta_block_t kv NKV_STACK_ALIGN4;
    uint32_t kv_addr;
    int ret;

    if (handle == NULL || key == NULL) {
        ERROR_LOG("remove failed: invalid args");
        return -1;
    }
    if (handle->is_init == 0u) {
        ERROR_LOG("remove failed: ctrl not initialized");
        return -1;
    }
    if (handle->desc.opt == NULL || handle->desc.opt->write == NULL) {
        return -1;
    }

    ret = nkv_find_last_kv_meta_block(handle, key, &kv, &kv_addr);
    if (ret != 0) {
        if (ret == -4) {
            WARN_LOG("remove miss: key=%s", key);
        }
        return -1;
    }

    if (kv.kv_status.active == 0u) {
        DEBUG_LOG("remove noop: key=%s already inactive addr=%u", key, (unsigned)kv_addr);
        return 0;
    }

    kv.kv_status.active = 0u;
    if (handle->desc.opt->write(handle->desc.opt_ctx, kv_addr, (uint8_t *)&kv, sizeof(kv))) {
        ERROR_LOG("remove failed: write kv meta key=%s addr=%u", key, (unsigned)kv_addr);
        return -1;
    }

    INFO_LOG("remove ok: key=%s addr=%u", key, (unsigned)kv_addr);
    return 0;
}
