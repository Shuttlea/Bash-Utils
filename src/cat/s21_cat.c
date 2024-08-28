#include <getopt.h>
#include <stdio.h>
#include <string.h>

int reading_file(FILE *fp, int flags[6], int *num_line, char *prev_ch,
                 int new_line);
void flag_trigger(int res, int flags[6], int *argc);

/*
  flags array:
    0. -b (GNU: --number-nonblank)
    1. -e предполагает и -v (GNU only: -E то же самое, но без применения -v)
    2. -n (GNU: --number)
    3. -s (GNU: --squeeze-blank)
    4. -t предполагает и -v (GNU: -T то же самое, но без применения -v)
    5. -v
*/

int main(int argc, char *argv[]) {
  int flags[6] = {0};
  int num_line = 0, res, new_line = 1;
  char prev_ch = 10;
  static struct option long_options[] = {{"number-nonblank", 0, 0, 'b'},
                                         {"number", 0, 0, 'n'},
                                         {"squeeze-blank", 0, 0, 's'},
                                         {0, 0, 0, 0}};
  while ((res = getopt_long(argc, argv, "bEensTtv", long_options, NULL)) != -1)
    flag_trigger(res, flags, &argc);
  for (int i = optind; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL)
      fprintf(stderr, "cat: %s: No such file or directory\n", argv[i]);
    else {
      if (flags[3] && prev_ch != 10) {
        if (flags[1]) printf("$");
        printf("\n");
        prev_ch = 10;
      }
      new_line = reading_file(fp, flags, &num_line, &prev_ch, new_line);
      fclose(fp);
    }
  }
  return 0;
}

void flag_trigger(int res, int flags[6], int *argc) {
  switch (res) {
    case 'b':
      flags[0] = 1;
      flags[2] = 0;
      break;
    case 'e':
      flags[1] = 1;
      flags[5] = 1;
      break;
    case 'E':
      flags[1] = 1;
      break;
    case 'n':
      if (!flags[0]) flags[2] = 1;
      break;
    case 's':
      flags[3] = 1;
      break;
    case 't':
      flags[4] = 1;
      flags[5] = 1;
      break;
    case 'T':
      flags[4] = 1;
      break;
    case 'v':
      flags[5] = 1;
      break;
    default:
      fprintf(stderr, "Try 'cat --help' for more information.\n");
      *argc = 0;
      break;
  }
}

int reading_file(FILE *fp, int flags[6], int *num_line, char *prev_ch,
                 int new_line) {
  char ch;
  while (fscanf(fp, "%c", &ch) != EOF) {
    if (ch == 10 && flags[3] == 1) {
      if (new_line > 1) continue;
      new_line++;
    } else
      new_line = 0;
    if (*prev_ch == 10 && flags[2] == 1) {
      *num_line = *num_line + 1;
      printf("%6d\t", *num_line);
    }
    if (*prev_ch == 10 && ch != 10 && flags[0] == 1) {
      *num_line = *num_line + 1;
      printf("%6d\t", *num_line);
    }
    if (ch == 10 && flags[1] == 1) printf("$");
    if (flags[4] == 1 && ch == 9)
      printf("^I");
    else if (flags[5] && ((ch >= 0 && ch <= 8) || (ch >= 11 && ch <= 31)))
      printf("^%c", ch + 64);
    else if (flags[5] && ch == 127)
      printf("^?");
    else if (flags[5] && ch <= -97)
      printf("M-^%c", ch + 192);
    else if (flags[5] && (ch >= -96 && ch <= -2))
      printf("M-%c", ch + 128);
    else if (flags[5] && ch == -1)
      printf("M-^?");
    else
      printf("%c", ch);
    *prev_ch = ch;
  }
  return new_line;
}
