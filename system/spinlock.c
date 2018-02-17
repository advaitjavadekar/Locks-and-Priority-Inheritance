#include <xinu.h>

void sl_init (sl_lock_t *l) {
  l->taken = 0;
}

void sl_lock (sl_lock_t *l) {
  while(test_and_set(&l->taken,1)==1);
}

void sl_unlock (sl_lock_t *l) {
  l->taken = 0;
}
