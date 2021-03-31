#include <stdio.h>
#include"stdlib.h"
#include"stdbool.h"

struct map {
        unsigned m_size;    // length of free space
        char *m_addr;   // start address of free space
        struct map* prev;
        struct map* next;
};



void init(struct map** head, char** q, unsigned n){
    char* p = (char*) malloc(n);
    *q = p;
    struct map* mp = (struct map*)malloc(sizeof(struct map));
    //static struct map mp;
    mp->m_size = n;
    mp->m_addr = p;
    mp->prev = mp;
    mp->next = mp;
    *head = mp;
    //printf("%u, %u, %u\n", (*head)->m_size, (*head)->m_addr, *head);
}

char *lmalloc(struct map **mp, unsigned size)
{
    //printf("%u\n", *mp);
    struct map *bp = *mp;
    //register struct map *bp = *mp;
    struct map *del;
    char* t;
    do{
        //printf("%u, %u, %u\n", bp->m_addr, bp->m_size, size);
        t = bp->m_addr;
        if(bp->m_size > size){
            //printf("Previous %u\n", bp->m_addr);
	        bp->m_addr += size;
            //printf("After %u\n", bp->m_addr);
	        bp->m_size -= size;
	        *mp = bp;
	        //printf("Type 0.\n");
            return t;
        }
        else if(bp->m_size == size){
            bp->m_addr += size;
	        bp->m_size -= size;
            if(bp->next != bp){
                bp->prev->next = bp->next;
                bp->next->prev = bp->prev;
                del = bp;
                bp = bp->next;
                free(del);
            }
            *mp = bp;
            return t;
        }
        else{
            bp = bp->next;
        }
    }while(bp != *mp);

    return NULL;
}

bool lfree(struct map** mp, char* sp, unsigned size){
    struct map* bp = *mp;

    //process the only block
    if(bp->prev == bp && bp->m_size == 0){
        bp->m_addr = sp;
        bp->m_size = size;
        return true;
    }

    // process the final block
    while(bp->next->m_addr > bp->m_addr){
        bp = bp->next;
    }

    if(sp == bp->m_addr + bp->m_size){
        bp->prev->m_size += size;
        return true;
    }
    else if(sp > bp->m_addr + bp->m_size){
        struct map* tmp = (struct map*)malloc(sizeof(struct map));
        //static struct map tmp;
        tmp->m_size = size;
        tmp->m_addr = sp;
        //printf("%u, %u, %u, %u\n", size, tmp.m_size, sp, tmp.m_addr);

        tmp->prev = bp->prev;
        tmp->next = bp;

        bp->prev->next = tmp;
        bp->prev = tmp;
        return true;
    }
    else if(sp > bp->m_addr){
        return false;
    }

    //normal process
    while(bp->prev->m_addr < bp->m_addr){
        bp = bp->prev;
    }

    while(bp->m_addr < sp){
        bp = bp->next;
    }

    if((bp->prev->m_addr + bp->prev->m_size > sp && bp->prev != bp && bp->prev->m_addr <= bp->m_addr) || sp + size > bp->m_addr){
        return false;
    }

    struct map *del;

    if(bp->m_addr == sp + size && bp->prev->m_addr + bp->prev->m_size == sp){
        //printf("Type 0.\n");
        bp->prev->m_size += size + bp->m_size;

        bp->prev->next = bp->next;
        bp->next->prev = bp->prev;
        del = bp;
        bp = bp->next;
        *mp = bp->prev;
        free(del);
        return true;
        //printf("test\n");
    }
    else if(bp->m_addr == sp + size){
        //f 3printf("Type 1.\n");
        bp->m_addr = sp;
        bp->m_size += size;
        return true;
    }
    else if(bp->prev->m_addr + bp->prev->m_size == sp){
        //printf("Type 2.\n");
        bp->prev->m_size += size;
        return true;
    }
    else{
        //printf("Type 3.\n");
        struct map* tmp = (struct map*)malloc(sizeof(struct map));
        //static struct map tmp;
        tmp->m_size = size;
        tmp->m_addr = sp;
        //printf("%u, %u, %u, %u\n", size, tmp.m_size, sp, tmp.m_addr);

        tmp->prev = bp->prev;
        tmp->next = bp;

        bp->prev->next = tmp;
        bp->prev = tmp;
        return true;
    }
}

void printFS(struct map* mp, char* q){
    struct map* bp = mp;
    while(bp->prev->m_addr < bp->m_addr){
        bp = bp->prev;
    }
    const struct map* hd = bp;
    printf("Free space:\n");
    do{
        //printf("bp=%u, hd=%u\n", bp, hd);
        printf("addr=%u, size=%u\n", bp->m_addr - q, bp->m_size);
        bp = bp->next;
    }while(bp != hd);
    printf("\n");
}

int main(){
    struct map* head;
    char* q;

    unsigned n = 100;
    init(&head, &q, n);

    char c;
    unsigned size;
    char* addr;
    unsigned vaddr;
    bool flag;

    while(1){
        do{
            c=getchar();
        }while(c=='\n'||c=='\t'||c==' ');

        if(c=='m'){
            scanf("%u", &size);
            addr = lmalloc(&head, size);
            if(addr){
                vaddr = addr - q;
                printf("Allocate space address: %u, size:%u\n", vaddr, size);
                printFS(head, q);
            }
            else{
                printf("Error: Allocation failed.\n");
            }
        }
        else if(c=='f'){
            scanf("%u %u", &vaddr, &size);
            if(vaddr + size <= n && size > 0 && vaddr < n){
                addr = q + vaddr;
                flag = lfree(&head, addr, size);
                if(flag){
                    printFS(head, q);
                }
                else{
                    printf("Error: Input is wrong.\n");
                }
            }
            else{
                printf("Error: Input is wrong.\n");
            }
        }
        else if(c=='q'){
            break;
        }
        else{
            printf("Error: Input is wrong.\n");
        }
    }

    free(q);
    struct map* bp;
    struct map* hd = head;
    do{
        bp = head;
        head = head->next;
        free(bp);
        //printf("test0");
    }while(head!=hd);

    return 0;
}
