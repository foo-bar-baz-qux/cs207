#include "bloom.h"
#include <stdio.h>
#include <time.h>
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

int * createRandArray(int N) {
  int i;
  int *arr = (int *) malloc(N * sizeof(int));

  if( arr == NULL ) return NULL; // Error in allocataing array


  for( i=0; i<N; i++ ) {
    arr[i] = rand()%1000000;
  }

  return arr;
}

void experiment(int *inp1, int *inp2) {
  int i, j, fp = 0;
  uint64_t bSet;
  bloom_filter_t testH;

  bloom_init(&testH, 1000); // Init the bloom filter

  for( i = 0; i < 100; i++ ) bloom_add(&testH, inp1[i]); // Add numbers from inp1
  bSet = countBitsSet(&testH); // Get bits set in the bloom filter (occupancy)

  for( i = 0; i < 100; i++ ) {
    int ok = 1; // Check that the inp2 number is not in inp1
    for( j = 0; j <100; j++ ) {
      if( inp2[i] == inp1[j] ) {
        ok = 0;
        break;
      }
    }
    if( !ok ) break;

    if( bloom_check(&testH, inp2[i]) != 0 ) fp++;
  }

  // Reference: http://stackoverflow.com/questions/9225567/how-to-print-a-int64-t-type-in-c
  printf("Occupancy: %" PRIu64 ", FP: %d\n", bSet, fp);
}

int main() {
  int *inp1, *inp2;
  srand(time(NULL)); // Seed the random generator

  // Testing the bloom filter
  // --------------------------------

  // int tN = 6, i;
  // uint64_t realN[1000], rN = 100;
  // // uint64_t testK[6] = {0, 1, 2, 3, 13, 97};
  // bloom_filter_t testH;
  // bloom_init(&testH, 1000);
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

  // bloom_destroy(&testH);

  // Running the experiments
  // --------------------------------
  inp1 = createRandArray(100);
  inp2 = createRandArray(100);

  experiment(inp1, inp2);

  return 0;
}
