// #ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Memory physical module mm/mm-memphy.c
 */

#include "mm.h"
#include <stdlib.h>
#include <stdio.h>

pthread_mutex_t lock_mem;

FILE *file;

/*
 *  MEMPHY_mv_csr - move MEMPHY cursor
 *  @mp: memphy struct
 *  @offset: offset
 */
int MEMPHY_mv_csr(struct memphy_struct *mp, int offset)
{
  int numstep = 0;

  mp->cursor = 0;
  while (numstep < offset && numstep < mp->maxsz)
  {
    /* Traverse sequentially */
    mp->cursor = (mp->cursor + 1) % mp->maxsz;
    numstep++;
  }

  return 0;
}

/*
 *  MEMPHY_seq_read - read MEMPHY device
 *  @mp: memphy struct
 *  @addr: address
 *  @value: obtained value
 */
int MEMPHY_seq_read(struct memphy_struct *mp, int addr, BYTE *value)
{
  if (mp == NULL)
    return -1;

  if (!mp->rdmflg)
    return -1; /* Not compatible mode for sequential read */

  MEMPHY_mv_csr(mp, addr);
  *value = (BYTE)mp->storage[addr];

  return 0;
}

/*
 *  MEMPHY_read read MEMPHY device
 *  @mp: memphy struct
 *  @addr: address
 *  @value: obtained value
 */
int MEMPHY_read(struct memphy_struct *mp, int addr, BYTE *value)
{
  if (mp == NULL)
    return -1;

  if (mp->rdmflg)
    *value = mp->storage[addr];
  else /* Sequential access device */
    return MEMPHY_seq_read(mp, addr, value);

  return 0;
}

/*
 *  MEMPHY_seq_write - write MEMPHY device
 *  @mp: memphy struct
 *  @addr: address
 *  @data: written data
 */
int MEMPHY_seq_write(struct memphy_struct *mp, int addr, BYTE value)
{

  if (mp == NULL)
    return -1;

  if (!mp->rdmflg)
    return -1; /* Not compatible mode for sequential read */

  MEMPHY_mv_csr(mp, addr);
  // mp->storage[addr] = (uint8_t)value;
  mp->storage[addr] = value;

  return 0;
}

/*
 *  MEMPHY_write-write MEMPHY device
 *  @mp: memphy struct
 *  @addr: address
 *  @data: written data
 */
int MEMPHY_write(struct memphy_struct *mp, int addr, BYTE data)
{
  pthread_mutex_lock(&lock_mem);
  if (mp == NULL)
  {
    pthread_mutex_unlock(&lock_mem);
    return -1;
  }

  if (mp->rdmflg)
  {
    mp->storage[addr] = data;
    pthread_mutex_unlock(&lock_mem);
  }
  else /* Sequential access device */
  {
    pthread_mutex_unlock(&lock_mem);
    return MEMPHY_seq_write(mp, addr, data);
  }

  return 0;
}

/*
 *  MEMPHY_format-format MEMPHY device
 *  @mp: memphy struct
 */
int MEMPHY_format(struct memphy_struct *mp, int pagesz)
{
  /* This setting come with fixed constant PAGESZ */
  int numfp = mp->maxsz / pagesz;
  struct framephy_struct *newfst, *fst;
  int iter = 0;

  if (numfp <= 0)
    return -1;

  /* Init head of free framephy list */
  fst = malloc(sizeof(struct framephy_struct));
  fst->fpn = iter;
  mp->free_fp_list = fst;

  /* We have list with first element, fill in the rest num-1 element member*/
  for (iter = 1; iter < numfp; iter++)
  {
    newfst = malloc(sizeof(struct framephy_struct));
    newfst->fpn = iter;
    newfst->fp_next = NULL;
    fst->fp_next = newfst;
    fst = newfst;
  }

  return 0;
}

/*  return frame number of first free frame in free frame list to retfpn
 *   return 0 if success, -1 if there no free frame in RAM
 */
int MEMPHY_get_freefp(struct memphy_struct *mp, int *retfpn)
{
  pthread_mutex_lock(&lock_mem);
  struct framephy_struct *fp = mp->free_fp_list;

  if (fp == NULL)
  {
    pthread_mutex_unlock(&lock_mem);
    return -1;
  }

  *retfpn = fp->fpn;
  mp->free_fp_list = fp->fp_next;

  /* MEMPHY is iteratively used up until its exhausted
   * No garbage collector acting then it not been released
   */
  free(fp);
  pthread_mutex_unlock(&lock_mem);
  return 0;
}

int MEMPHY_dump(struct memphy_struct *mp)
{
  /*TODO dump memphy contnt mp->storage
   *     for tracing the memory content
   */
  pthread_mutex_lock(&lock_mem);
  /*
    if (mp->rdmflg == 1) {
      for (int i = 0; i<mp->maxsz; ++i) {
        if (i%256 == 0) printf("\n\t\tFrame %d", PAGING_PAGE_ALIGNSZ(i)/PAGING_PAGESZ);
        if (i%32 == 0) printf("\n");
        printf("%d ", (int)(mp->storage[i]));
        if (i == mp->maxsz - 1) printf("\n");
      }
    } else {
      // do nothing
    }
   */
  file = fopen("RAM_status.txt", "w");
  struct framephy_struct *frameit = mp->used_fp_list;
  while (frameit != NULL) {
#ifdef DUMP_TO_FILE
    fprintf(file, "\t\t Frame %08x", frameit->fpn);
#else
    printf("\t\t Frame %d", frameit->fpn);
#endif
    for (int off = 0; off < PAGING_PAGESZ; ++off) {
      if (off % 32 == 0)  {
#ifdef DUMP_TO_FILE
        fprintf(file, "\n");
#else
        printf("\n");
#endif
      }
#ifdef DUMP_TO_FILE
      fprintf(file, "%d ", mp->storage[frameit->fpn * PAGING_PAGESZ + off]);
#else
      printf("%d ", mp->storage[frameit->fpn * PAGING_PAGESZ + off]);
#endif
    }
#ifdef DUMP_TO_FILE
    fprintf(file, "\n");
#else
    printf("\n");
#endif
    frameit = frameit->fp_next;
  }

  fclose(file);
  pthread_mutex_unlock(&lock_mem);
  return 0;
}

int MEMPHY_put_freefp(struct memphy_struct *mp, int fpn)
{
  struct framephy_struct *fp = mp->free_fp_list;
  struct framephy_struct *newnode = malloc(sizeof(struct framephy_struct));

  /* Create new node with value fpn */
  newnode->fpn = fpn;
  newnode->fp_next = fp;
  mp->free_fp_list = newnode;

  return 0;
}

int MEMPHY_put_usedfp(struct memphy_struct *mp, int fpn)
{

  pthread_mutex_lock(&lock_mem);
  // struct framephy_struct *fp = (*mp)->used_fp_list;

  struct framephy_struct *newnode = malloc(sizeof(struct framephy_struct));

  /* Create new node with value fpn */
  newnode->fpn = fpn;
  // newnode->fp_next = fp;
  newnode->fp_next = mp->used_fp_list;
  mp->used_fp_list = newnode;

  pthread_mutex_unlock(&lock_mem);

  return 0;
}

/*
 *  Init MEMPHY struct
 */
int init_memphy(struct memphy_struct *mp, int max_size, int randomflg)
{
  mp->storage = (BYTE *)malloc(max_size * sizeof(BYTE));
  mp->maxsz = max_size;

  mp->used_fp_list = NULL;

  MEMPHY_format(mp, PAGING_PAGESZ);

  mp->rdmflg = (randomflg != 0) ? 1 : 0;

  if (!mp->rdmflg) /* Not Ramdom acess device, then it serial device*/
    mp->cursor = 0;

  return 0;
}

// #endif
