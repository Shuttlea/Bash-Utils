#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct flags_struct {
  int e, f, i, v, c, l, n, h, s, o;
};

void flag_detector(int res, char pattern[10000], struct flags_struct* flags,
                   int* pattern_file);
void grep(struct flags_struct* flags, char* pattern, int optind, int argc,
          char* argv[]);
void print_grep(FILE* fp, struct flags_struct* flags, regex_t preg,
                char* file_name, int file_count);
void NotFlagCFunction(int file_count, struct flags_struct* flags,
                      char* file_name, int string_count,
                      regmatch_t pmatch[10000], char string_from_file[2500]);

int main(int argc, char* argv[]) {
  int pattern_file = 1;
  char pattern[10000];
  struct flags_struct flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  pattern[0] = '\0';
  int res;
  while ((res = getopt_long(argc, argv, "e:f:ivclnhso", NULL, NULL)) != -1) {
    flag_detector(res, pattern, &flags, &pattern_file);
  }
  if (!flags.e && !flags.f) strcat(pattern, argv[optind++]);
  if (strlen(pattern) > 0 && pattern_file) {
    grep(&flags, pattern, optind, argc, argv);
  }
  return 1;
}

void flag_detector(int res, char pattern[10000], struct flags_struct* flags,
                   int* pattern_file) {
  char string_from_file[2500];
  FILE* fp;
  switch (res) {
    case 'e':
      if (pattern[0] != '\0') strcat(pattern, "|");
      strcat(pattern, optarg);
      flags->e = 1;
      break;
    case 'f':
      fp = fopen(optarg, "r");
      if (fp != NULL) {
        while (fgets(string_from_file, 2499, fp) != NULL) {
          if (strlen(pattern) > 0) strcat(pattern, "|");
          if (string_from_file[strlen(string_from_file) - 1] == '\n')
            string_from_file[strlen(string_from_file) - 1] = '\0';
          strcat(pattern, string_from_file);
        }
        fclose(fp);
      } else {
        printf("s21_grep: %s: No such file or directory\n", optarg);
        *pattern_file = 0;
      }
      flags->f = 1;
      break;
    case 'i':
      flags->i = REG_ICASE;
      break;
    case 'v':
      flags->v = REG_NOMATCH;
      break;
    case 'c':
      flags->c = 1;
      break;
    case 'l':
      flags->l = 1;
      break;
    case 'n':
      flags->n = 1;
      break;
    case 'h':
      flags->h = 1;
      break;
    case 's':
      flags->s = 1;
      break;
    case 'o':
      flags->o = 1;
      break;
  }
}

void grep(struct flags_struct* flags, char* pattern, int optind, int argc,
          char* argv[]) {
  FILE* fp;
  regex_t preg;
  regcomp(&preg, pattern, flags->i | REG_EXTENDED);
  for (int i = optind; i < argc; i++) {
    fp = fopen(argv[i], "r");
    if (fp != NULL) {
      print_grep(fp, flags, preg, argv[i], argc - optind);
      fclose(fp);
    } else if (flags->s == 0)
      fprintf(stderr, "s21_grep: %s: No such file or directory\n", argv[i]);
  }
  regfree(&preg);
}

void print_grep(FILE* fp, struct flags_struct* flags, regex_t preg,
                char* file_name, int file_count) {
  int match_count = 0, string_count = 0;
  char string_from_file[2500];
  regmatch_t pmatch[10000];
  while (fgets(string_from_file, 2499, fp) != NULL) {
    string_count++;
    while (regexec(&preg, string_from_file, 10000, pmatch, 0) == flags->v) {
      if (flags->l) {
        printf("%s\n", file_name);
        return;
      }
      if (!flags->c) {
        NotFlagCFunction(file_count, flags, file_name, string_count, pmatch,
                         string_from_file);
      } else {
        string_from_file[0] = '\0';
        match_count++;
      }
      if (flags->v == REG_NOMATCH) break;
    }
  }
  if (flags->c && !flags->l) {
    if (file_count > 1 && flags->h == 0) printf("%s:", file_name);
    printf("%d\n", match_count);
  }
}

void NotFlagCFunction(int file_count, struct flags_struct* flags,
                      char* file_name, int string_count,
                      regmatch_t pmatch[10000], char string_from_file[2500]) {
  if (file_count > 1 && flags->h == 0 && !(flags->o && flags->v))
    printf("%s:", file_name);
  if (flags->n) printf("%d:", string_count);
  if (flags->o) {
    if (!flags->v) {
      for (int j = pmatch[0].rm_so; j < pmatch[0].rm_eo; j++)
        printf("%c", string_from_file[j]);
      if (pmatch[0].rm_eo != 0) printf("\n");
      size_t lenth = strlen(string_from_file);
      for (size_t i = pmatch[0].rm_eo; i <= lenth; i++)
        string_from_file[i - pmatch[0].rm_eo] = string_from_file[i];
    }
  } else {
    printf("%s", string_from_file);
    if (string_from_file[strlen(string_from_file) - 1] != '\n') printf("\n");
    string_from_file[0] = '\0';
  }
}
