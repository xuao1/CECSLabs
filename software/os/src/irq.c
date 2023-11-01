#include <debug.h>
#include <proc.h>
#define ECALL_FROM_M 0xb
#define SYSCALL_YIELD 0xffffffff

// typedef struct {
//   enum {
//     EVENT_NULL = 0,
//     EVENT_YIELD, EVENT_SYSCALL, EVENT_PAGEFAULT, 
//     EVENT_IRQ_TIMER, EVENT_IRQ_IODEV,
//     EVENT_ERROR,
//   } event;
//   uintptr_t cause, ref;
//   const char *msg;
// } Event;

void syscall_handle(Context *c);
static Context* __event_handle(Event e, Context* c);

Context* __irq_handle(Context *c) {
  Event ev = {0};
  switch (c->mcause) {
    // Lab7 TODO: implement EVENT_SYSCALL and EVENT_YIELD

    case ECALL_FROM_M: {
      switch(c->gpr[17]){
        case EVENT_SYSCALL : {
          ev.event = EVENT_SYSCALL;
          ev.cause = c->gpr[17];
          ev.ref = c->gpr[10];
        } break;
        case EVENT_YIELD : {
          ev.event = EVENT_YIELD;
          ev.cause = c->gpr[17];
          ev.ref = c->gpr[10];
        } break;
      } break;
    }

    default: ev.event = EVENT_ERROR; break;
  }
  c = __event_handle(ev, c);
  assert(c != NULL);
  return c;    
}


static Context* __event_handle(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD:
      yield();
      c->mepc += 4;
      break;
    case EVENT_SYSCALL:
      syscall_handle(c);
      c->mepc += 4;
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

extern void __trap_vector(void);

void yield() {
  asm volatile("li a7, -1; ecall");
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  asm volatile("csrw mtvec, %0" : : "r"(__trap_vector));
}
