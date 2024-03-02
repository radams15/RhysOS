#include "keyboard.h"

#include "tty.h"

unsigned char kbd_mtrx[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '+', /*'´' */0, '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '<',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '-',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,  '<',
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

#define KBD_BUFFER_SIZE 4

unsigned char buffer[KBD_BUFFER_SIZE];
char front = -1;
char rear = -1;

int is_full() {
  if ((front == rear + 1) || (front == 0 && rear == KBD_BUFFER_SIZE - 1)) return 1;
  return 0;
}

int is_empty() {
  if (front == -1) return 1;
  return 0;
}

void kbd_key_press(char scan) {
    if((unsigned char) scan > 80)
        return;

    unsigned char c = kbd_mtrx[scan];

  if (is_full()) {
      kbdbuf_get();
  }

  if (front == -1) front = 0;
  rear = (rear + 1) % KBD_BUFFER_SIZE;
  buffer[rear] = c;
}

char kbdbuf_get() {
  if (is_empty()) {
    return -1;
  } else {
      print_string("Dequeue");
    int element = buffer[front];
    if (front == rear) {
      front = -1;
      rear = -1;
    }
    // Q has only one element, so we reset the
    // queue after dequeing it. ?
    else {
      front = (front + 1) % KBD_BUFFER_SIZE;
    }
    return element;
  }
}
