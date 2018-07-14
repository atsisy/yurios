#include <mm.h>

static heap_header *heap_list_header;

void heap_init(void *begin_addr, size_t heap_size)
{
        printk("init kernel heap ");
        heap_list_header = begin_addr;
        heap_list_header->s.ptr = heap_list_header;
        heap_list_header->s.size = heap_size;
        printk("done\n");
}

void *kr_kmalloc(size_t size)
{
        heap_header *tmp, *prev = heap_list_header;
        void *addr;

        const size_t real_size = size + sizeof(heap_header);
        
        for(tmp = prev->s.ptr;;prev = tmp, tmp = tmp->s.ptr){
                if(tmp->s.size >= size){
                        if(tmp->s.size == real_size){
                                prev->s.ptr = tmp->s.ptr;
                        }else{
                                tmp->s.size -= real_size;
                                tmp += tmp->s.size;
                                tmp->s.size = real_size;
                        }
                        heap_list_header = prev;
                        return (void *)(tmp + sizeof(heap_header));
                }
                if(tmp == heap_list_header)
                        return NULL;
        }

        return NULL;
}

void kr_kfree(const void *addr)
{
        heap_header *base_p, *p;

        base_p = (heap_header *)addr - sizeof(heap_header);
        for(p = heap_list_header;!(base_p > p && base_p < p->s.ptr);p = p->s.ptr)
                if(p >= p->s.ptr && ((base_p > p || base_p < p->s.ptr)))
                        break;

        puts("hello");

        if(base_p + base_p->s.size == p->s.ptr){
                base_p->s.size += ((heap_header *)p->s.ptr)->s.size;
                base_p->s.ptr = p->s.ptr;
        }else{
                base_p->s.ptr = p->s.ptr;
        }

        if(p + p->s.size == base_p){
                p->s.size += base_p->s.size;
                p->s.ptr = base_p->s.ptr;
        }else{
                p->s.ptr = base_p;
        }

        heap_list_header = p;
}
