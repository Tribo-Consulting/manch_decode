const int ringsize = 8;

uint8_t head = 0;
uint8_t tail = 0;
byte ring[ringsize];

// Put something into the buffer. Returns 0 when the buffer was full,
// 1 when the stuff was put sucessfully into the buffer
int enqueue (byte val) {
  int newtail = (tail + 1) % ringsize;
  if (newtail == head) {
     // Buffer is full, do nothing
     return 0;
  }
  else {
     ring[tail] = val;
     tail = newtail;
     return 1;
  }
}

// Return number of elements in the queue.
int queuelevel () {
   return tail - head + (head > tail ? ringsize : 0);
}

// Get something from the queue. 0 will be returned if the queue
// is empty
byte dequeue () {
  if (head == tail) {
     return 0;
  }
  else {
     byte val = ring[head];
     head  = (head + 1) % ringsize;
     return val;
  }
}

