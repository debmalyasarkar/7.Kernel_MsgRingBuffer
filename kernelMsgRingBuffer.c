/* Program to print the contents of Kernel Message Ring Buffer in console */
/* Similar to dmesg utility output */
#include <stdio.h>
#include <stdlib.h>
#include <sys/klog.h>

/* Kernel Message Ring Buffer Command Set */
#define SYSLOG_ACTION_CLOSE		0
#define SYSLOG_ACTION_OPEN		1
#define SYSLOG_ACTION_READ		2
#define SYSLOG_ACTION_READ_ALL		3
#define SYSLOG_ACTION_READ_CLEAR	4
#define SYSLOG_ACTION_CLEAR		5
#define SYSLOG_ACTION_CONSOLE_OFF	6
#define SYSLOG_ACTION_CONSOLE_ON	7
#define SYSLOG_ACTION_CONSOLE_LEVEL	8
#define SYSLOG_ACTION_SIZE_UNREAD	9
#define SYSLOG_ACTION_SIZE_BUFFER	10

int main(void)
{
  char *buf  = NULL ;
  int kbufSz = 0, rdLen = 0;
  int last = '\n';
  int in = 0;

  /* Find the size of the Kernel Message Ring Buffer */
  kbufSz = klogctl(SYSLOG_ACTION_SIZE_BUFFER, NULL, 0);
  printf("Kernel Message Ring Buffer Size = %d\n",kbufSz);

  /* Calculate the padding for memory alignment */
  if(kbufSz < 16 * 1024)
    kbufSz = 16 * 1024;
  else if(kbufSz > 16 * 1024 * 1024)
    kbufSz = 16 * 1024 * 1024;
  
  /* Allocate memory dynamically to save the Kernel Log data */
  buf = malloc(kbufSz);
  if(NULL == buf)
  {
    perror("malloc");
    return -1;
  }

  /* Read the contents of the Kernel Message Ring Buffer */
  rdLen = klogctl(SYSLOG_ACTION_READ_ALL, buf, kbufSz);
  if(0 == rdLen)
  {
    printf("Buffer is Empty\n");
    free(buf);
    return -1;
  }
  else if(0 > rdLen)
  {
    perror("klogctl");
    free(buf);
    return -1;
  }
  else
    printf("No of Bytes Read = %d\n",rdLen);

  /* Skip <#> at the start of lines in the log */
  while (1) 
  {
    if(last == '\n' && buf[in] == '<')
    {
      in += 3;
      if(in >= rdLen)
        break;
    }
    last = buf[in];
    putchar(last);
    in++;
    if(in >= rdLen)
      break;
  }

  /* Make sure we end with a newline */
  if (last != '\n')
    putchar('\n');

  free(buf);
  return 0;
}

