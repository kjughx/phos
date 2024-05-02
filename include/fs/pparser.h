#ifndef _PATH_PARSER_H_
#define _PATH_PARSER_H_

struct path_root {
    int drive_no;
    struct path_part* first;
};

struct path_part {
    const char* part;
    struct path_part* next;
};

struct path_root* pparser_parse(const char* path, const char* cwd);
void pparser_free(struct path_root* root);

#endif /* _PATH_PARSER_H_ */
