# NKV (当前实现说明)

本文档描述 `src/framework/lib/nkv/nkv.c` 的**当前实际实现**，不描述计划中的完整形态。

## 1. 设计概览

当前 NKV 把 `nkv_area` 固定分为两个 section：

- `section0`: 从 `nkv_area_start_addr` 开始
- `section1`: 从 `nkv_area_start_addr + nkv_area_size/2` 开始
- `section_size = nkv_area_size / 2`

仅维护一个 `active_section_meta_block` 作为当前读写段。

## 2. 存储格式

详见 `private/meta_block.h`，核心结构如下：

- `section_meta_block_t`
  - `type = META_BLOCK_TYPE_SECTION`
  - `section_info`（含 `area_size/section_index/section_size/section_start_addr/version`）
  - `info_crc16`
  - `section_status.active`
- `kv_meta_block_t`
  - `type = META_BLOCK_TYPE_KV`
  - `kv_info`（`key_size/value_size/key_crc16/value_crc16`）
  - `info_crc16`
  - `kv_status`（`active` + `next_same_meta_block_offset`）

当前值数据布局：

1. `kv_meta_block_t`
2. `key` 字节串（含 `\0`）
3. `value` 原始字节

即并非固定 32B slot 结构，而是变长顺序写入。

## 3. CRC16 算法

当前代码使用如下 CRC16（`nkv_crc16`）：

- 初始值：`0x0000`
- 多项式：`0x8005`
- 按 bit 左移计算

用于：

- `section_info` 的校验（`section_meta_block.info_crc16`）
- `kv_info` 的校验（`kv_meta_block.info_crc16`）
- key/value 内容校验（`key_crc16` / `value_crc16`）

## 4. 初始化流程

`nkv_init(handle)` 逻辑：

1. 扫描 section0 的 section meta
2. 若不 active，再扫 section1
3. 校验 `section_size == nkv_area_size/2`
4. 通过顺序解析 kv 记录计算 `section_used`
5. 置 `is_init = 1`

返回值（当前实现）：

- `0` 成功
- `-1` 失败（参数、无 active 段、尺寸不一致等统一返回）

## 5. reset 流程

`nkv_ctrl_reset(handle)` 逻辑：

1. 调用 `erase(opt_ctx, nkv_area_start_addr, nkv_area_size)` 擦除全区
2. 构造并写入 section0 的 `section_meta_block_t`（active=1）
3. 更新 RAM 状态：
   - `active_section_meta_block = section0`
   - `section_used = sizeof(section_meta_block_t)`
   - `is_init = 1`

> 注意：当前不会写 section1 的 meta，也没有 active 段切换。

## 6. set/get/get_size 行为

### 6.1 `nkv_ctrl_set_value`

- 要求 `is_init=1`
- 构造新的 `kv_meta_block_t`
- 先查找同名 key 的“最后一个块”
  - 若存在：把旧块置 `active=0`，并将 `next_same_meta_block_offset` 指向新块
- 在 `section_used` 处顺序写入：
  - `kv_meta_block`
  - `key`
  - `value`
- 更新 `section_used`
- 剩余空间 `<512` 字节时打印 WARN 日志

### 6.2 `nkv_ctrl_get_value`

- 查找同名 key 链尾块
- 必须 `active=1`
- 从 `kv_meta_block_addr + sizeof(kv_meta_block_t) + key_size` 读取 value
- 输出大小为 `min(*size, value_size)`

### 6.3 `nkv_ctrl_get_value_size`

- 查找同名 key 链尾块
- 若存在且 active，返回 `value_size`
- 否则返回 `0`

## 7. 当前已实现/未实现接口

已实现：

- `nkv_ctrl_create`
- `nkv_ctrl_destroy`
- `nkv_init`
- `nkv_ctrl_reset`
- `nkv_ctrl_set_value`
- `nkv_ctrl_get_value`
- `nkv_ctrl_get_value_size`

未实现（头文件已声明，但 `nkv.c` 尚无实现）：

- `nkv_ctrl_sort`
- `nkv_ctrl_remove_kv`
- `nkv_ctrl_get_kv_cnt`
- `nkv_ctrl_get_key_by_index`

## 8. 使用约束与已知限制

1. `nkv_ctrl_create` 参数约束（当前实现）：
   - `desc/opt/read/write/erase` 不能为空
   - `nkv_area_size != 0`
   - `erase_unit_size != 0`
   - `(nkv_area_size / 2) % erase_unit_size == 0`
2. 当前没有 section 自动切换与空间回收（`sort` 未实现）。
3. 未做严格“写前剩余空间充足”保护，压测可能出现 section 用尽后读写异常。
4. `erase/write/read` 回调的返回约定为：`0` 成功，非 0 失败（按当前代码判断）。

## 9. 最小示例

```c
nkv_ctrl_opt_t opt = {
    .write = flash_write,
    .read  = flash_read,
    .erase = flash_erase,
};

nkv_ctrl_desc_t desc = {
    .opt = &opt,
    .opt_ctx = &flash_ctx,
    .erase_unit_size = 4096,
    .align_size = 4,
    .nkv_area_start_addr = 0,
    .nkv_area_size = 64 * 1024,
};

nkv_ctrl_handle_t h = nkv_ctrl_create(&desc);
nkv_ctrl_reset(h);   // 首次使用先格式化
nkv_init(h);

int v = 123;
nkv_ctrl_set_value(h, "k", &v, sizeof(v));
```