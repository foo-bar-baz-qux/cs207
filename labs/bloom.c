#include "bloom.h"
#include <stdio.h>
#include <inttypes.h>

index_t hash1(bloom_filter_t *B, key_t k) {
  // Reference: https://en.wikipedia.org/wiki/PJW_hash_function
  uint64_t hash;
  hash = k;
  hash += (hash << 10);
  hash ^= (hash >> 6);
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}
index_t hash2(bloom_filter_t *B, key_t k) {
  // Reference: https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html
  return (k*(k+3)) % B->size;
}

void set_bit(bloom_filter_t *B, index_t i) {
  B->table[i >> 6] |= ((uint64_t)1 << (i%64));
}
index_t get_bit(bloom_filter_t *B, index_t i) {
  return (((B->table[i >> 6]) >> (i % 64)) & 1);
}

void bloom_init(bloom_filter_t *B, index_t size_in_bits) {
  B->table = (index_t *) calloc( (size_in_bits/64 + 1), sizeof(index_t) );
  B->count = 0;
  B->size = size_in_bits;
}
void bloom_destroy(bloom_filter_t *B) {
  free(B->table);
}

int bloom_check(bloom_filter_t *B, key_t k) {
  int i;
  for ( i = 0; i < N_HASHES; i++ ) {
    if( get_bit(B, (hash1(B, k) + hash2(B, k) * i)%B->size) == 0 ) {
      return 0;
    }
  }
  return 1;
}
void bloom_add(bloom_filter_t *B, key_t k) {
  int i;
  for ( i = 0; i < N_HASHES; i++ ) {
    set_bit(B, ( (index_t) (hash1(B, k) + hash2(B, k) * i))%B->size);
  }
  B->count++;
}

uint64_t countBitsSet(bloom_filter_t *B) {
  int ret = 0;
  for( int i=0; i<B->size; i++ ) {
    if( get_bit(B, i) > 0 ) ret++;
  }
  return ret;
}

int main() {
  int tN = 6, i;
  uint64_t testK[6] = {0, 1, 2, 3, 13, 97};
  bloom_filter_t testH;
  bloom_init(&testH, 1000);
  // for( i=0; i<tN; i++ ) {
  //
  //   // Reference: http://stackoverflow.com/questions/9225567/how-to-print-a-int64-t-type-in-c
  //   // printf("Hashing %ul\n", testK[i]);
  //   // printf("Hash 1: %ul\n", hash1(NULL, testK[i]));
  //   // printf("Hash 2: %ul\n", hash2(NULL, testK[i]));
  //   // printf("-----------\n");
  //   // printf("%" PRIu64 ", ", hash1(NULL, testK[i]));
  //   // printf("\n");
  //   printf("%" PRIu64 ", ", hash2(&testH, testK[i]));
  // }

  for( i = 1; i <= 70; i++ ) {
    bloom_add(&testH, i);
  }

  printf("Bits set: %" PRIu64, countBitsSet(&testH));
  bloom_destroy(&testH);
  return 0;
}
