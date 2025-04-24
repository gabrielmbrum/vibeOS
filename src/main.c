#include "../include/process.h"
#include "../include/debug.h"
#include "../include/commons.h"

int main () {
  system("clear");
  printf("welecome to the vibeOS\n\n\njust feel the vibe...\n\n\n\n");
  for (int i = 1; i > 0; i++) {
    sleep(i);
    printf("%s\n\n\n", i % 2 == 0 ? "just feel the vibe..." : "are you feeling?");
  }
}