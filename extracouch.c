/*
 * A custom CouchDB query server written in C
 * The goal is to be as fast as possible, to check the overhead of the query server protocol.
 *
 * Build with: gcc -o extracouch -Wall extracouch.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Max line is 64MB */
#define MAX_LINE 67108864

void dlog(char *line)
{
  static char dst[MAX_LINE];
  strcpy(dst, line);
  for (int i = 0; dst[i] != '\0'; ++i)
  {
    if (dst[i] == '"')
      dst[i] = '\'';
    else if (dst[i] == '\n')
      dst[i] = '#';
  }
  fprintf(stdout, "[\"log\",\"%s\"]\n", dst);
  fflush(stdout);
}

void processline(char *line, size_t len)
{
  if (strncmp(line, "[\"map_doc\",", 11) == 0)
  {
    // dlog("[[[\"key\",\"value\"]]]\n");
    printf("[[[\"key\",\"value\"]]]\n");
  }
  else if (strncmp(line, "[\"reduce\",", 10) == 0)
  {
    // dlog("[true,[\"reduced\"]]\n");
    printf("[true,[\"reduced\"]]\n");
  }
  else if (strncmp(line, "[\"rereduce\",", 12) == 0)
  {
    // dlog("[true,[\"rereduced\"]]\n");
    printf("[true,[\"rereduced\"]]\n");
  }
  // else if (strncmp(line, "[\"reset\"", 8) == 0) {
  //   printf("true\n");
  // }
  else
  {
    // dlog("true\n");
    printf("true\n");
  }
}


int main(int argc, char **argv)
{
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  while ((nread = getline(&line, &len, stdin)) != -1)
  {
    // dlog(line);
    // printf("Retrieved line of length %zu:\n", nread);
    // fwrite(line, nread, 1, stdout);
    processline(line, nread);
    fflush(stdout);
  }

  free(line);
  return 0;
}


/*
BENCHMARKS:
-----------

indexing a database with:
- doc_count=41634
- doc_del_count=62044
- map() => emit("key", "value")
- reduce() => "reduced"

# Implemented using the built-in couchjs

> ms_javascript = (new Date('2000-01-01T11:34:51.000Z') - new Date('2000-01-01T11:32:47.000Z'))
124000 (124s)

# Implemented as custom Query Server in NodeJS

> ms_custom_js = (new Date('2000-01-01T11:37:02.000Z') - new Date('2000-01-01T11:35:39.000Z'))
83000 (83s) ... 92s (other runs)

# Implemented as custom Query Server in C

# ms_custom_c_run2?
> start_date="12:47:20"
'12:47:20'
> end_date="12:48:35"
'12:48:35'
> new Date("2000-01-01T" + end_date + ".000Z") - new Date("2000-01-01T" + start_date + ".000Z")
75000 (75s) ... 103s ... 79s (other runs)


> We can hope for 30% speed increase in this synthetic test (nothing justifying extra effort maybe)

Let's compare with a built-in reducer.
NodeJS: 102s
C: 86s

> It doesn't look like this is making such a difference.
*/
