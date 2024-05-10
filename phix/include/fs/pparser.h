#ifndef _PATH_PARSER_H_
#define _PATH_PARSER_H_

/* @brief Represents the root of a path */
struct path_root {
    int drive_no;
    struct path_part* first;
};

/* @brief Represents a branch/leaf of a path */
struct path_part {
    const char* part;
    struct path_part* next;
};

/* @brief Parse a path into a a @path_root and `path_part`s */
struct path_root* pparser_parse(const char* path, const char* cwd);

/* @brief Free a @path_root */
void pparser_free(struct path_root* root);

#endif /* _PATH_PARSER_H_ */
