# deepin-clone
Disk and partition backup/restore tool

# DIM文件格式详解

* 文件后缀名必须为 dim

dim文件元数据最大为的 24 * 1024 字节，位于文件开头。元数据格式：

|        | 版本  |  文件数量 | 文件1元数据...文件n元数据 |
|  ----  | ---- |  ----    |  ----  |
| 0xdd   | 1B   |   1B     |  ...   |

文件部分元数据格式：

|        | 文件名|  文件内容开始的偏移 | 文件内容结束的偏移 |
|  ----  | ---- |  ----    |  ----  |
| 0xdd   | 63B  |   8B     |   8B |

元数据结束后的16个字节为dim文件数据的校验和，其计算方法为：

````
md5_data // 保存参与md5计算的数据
md5_data.append(dim文件元数据)

首先将数据分块：
block_size = max(1024 * 1024, 文件大小 / 1000)

for (文件1...文件n) {
    seek（移动文件指针） 到文件的数据开头

    // pos 为表示文件指针当前位置
    while (pos < 文件数据的结尾 - block_size - 4) {
        block_index = 读取4个字节的数据
        block_index %= (block_size / 1024)
        old_pos = pos

        if (seek 到 pos + block_size * 1024) {
            md5_data.append(读取1024字节)
        } else {
            seek 失败结束计算
        }
    }

    md5_data.append(读取 min(文件数据的结尾 - pos, 1024 * 10))
}

check_sum = md5(md5_data)

````