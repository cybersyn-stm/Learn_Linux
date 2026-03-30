/* inspect_malloc.c
 * 演示如何读取 glibc malloc 分配后 chunk 的 header 中的 size 字段（仅用于调试/教学）
 * 注意：该方法依赖 glibc 的 malloc 实现细节，并非通用，直接访问内部分配器数据结构有安全风险。
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>

int main(void) {
    /* 要求分配的用户字节数 */
    size_t req = 123;

    /* malloc 返回的是用户可用的指针（user pointer） */
    void *p = malloc(req);
    if (!p) return 1; /* 分配失败 */

    printf("user ptr = %p\n", p);

    /*
     * 在 glibc 的 malloc 实现中，chunk 的 size 字段存储在用户指针之前一个 size_t 大小的位置。
     * 该 size 字段的低 3 位被用作标志（例如 PREV_INUSE / IS_MMAPPED / NON_MAIN_ARENA），
     * 因此读取后需要屏蔽低 3 位以获得按对齐后的 chunk 大小。
     */
    size_t raw = *((size_t*)((char*)p - sizeof(size_t)));
    printf("raw size field = 0x%zx\n", raw);

    /* 屏蔽低 3 位标志，得到包含头部的 chunk 大小（字节） */
    size_t chunk_size = raw & ~((size_t)0x7);
    printf("chunk_size (including header) = %zu\n", chunk_size);

    /* malloc_usable_size 返回实际可用于用户的字节数（不同于请求大小） */
    size_t usable = malloc_usable_size(p);
    printf("malloc_usable_size = %zu\n", usable);

    /* 根据 chunk_size 粗略估算用户 payload（减去前后 size 字段） */
    printf("estimated user payload from chunk_size = %zu\n", chunk_size - 2 * sizeof(size_t));

    free(p);
    return 0;
}
