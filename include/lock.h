struct sl_lock_t {
  uint32 taken;
};
typedef struct sl_lock_t sl_lock_t;

struct bwf_lock_t {
  uint32 taken;
  uint32 guard;
  qid16 twlist;
};
typedef struct bwf_lock_t bwf_lock_t;

struct al_lock_t {
  uint32 taken;
  uint32 guard;
  qid16 twlist; 
};
typedef struct al_lock_t al_lock_t;
 
struct pi_lock_t {
	uint32 pi_lock;
	uint32 pi_guard;
	pid32  lock_holder;
	qid16  pi_blist;
}; 
typedef struct pi_lock_t pi_lock_t;

 
