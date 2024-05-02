#include "pparser.h"
#include "config.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "status.h"
#include "string/string.h"

static inline int pparser_path_valid_format(const char* path) {
    int len = strnlen(path, PHOS_MAX_PATH);

    return (len >= 3 && is_digit(path[0]) && memcmp((void*)&path[1], ":/", 2) == 0);
}

static int pparser_get_drive_by_path(const char** path) {
    if (!pparser_path_valid_format(*path))
        return -EBADPATH;

    int drive_no = to_digit(*path[0]);

    /* Add three bytes to skip drive number: "0:/"*/
    *path += 3;

    return drive_no;
}

static struct path_root* pparser_create_root(int drive_no) {
    struct path_root* root = kzalloc(sizeof(struct path_root));
    root->drive_no = drive_no;

    return root;
}

static const char* pparser_get_path_part(const char** path) {
    char* path_part = kzalloc(PHOS_MAX_PATH);
    int i = 0;

    while (**path && **path != '/') {
        path_part[i++] = **path;
        (*path)++;
    }

    if (**path == '/') {
        /* Then skip the forward slash */
        *path += 1;
    }

    if (i == 0) {
        kfree(path_part);
        path_part = NULL;
    }

    return path_part;
}

static struct path_part* pparser_parse_path_part(struct path_part* prev, const char** path) {
    const char* path_part_str = pparser_get_path_part(path);
    if (!path_part_str)
        return 0;

    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = NULL;
    if (prev)
        prev->next = part;

    return part;
}

void pparser_free(struct path_root* root) {
    struct path_part* part = root->first;

    while (part) {
        struct path_part* next = part->next;
        kfree((void*)part->part);
        kfree(part);
        part = next;
    }

    kfree(root);
}

struct path_root* pparser_parse(const char* path, const char* cwd) {
    const char* tmp = path;
    int drive_no;

    struct path_root* root = NULL;
    if (strlen(path) > PHOS_MAX_PATH)
        return NULL;

    if ((drive_no = pparser_get_drive_by_path(&tmp) < 0))
        return NULL;

    root = pparser_create_root(drive_no);
    if (!root)
        return NULL;

    struct path_part* first = pparser_parse_path_part(NULL, &tmp);
    if (!first)
        goto out;

    root->first = first;

    struct path_part* part = pparser_parse_path_part(first, &tmp);
    while (part)
        part = pparser_parse_path_part(part, &tmp);

out:
    return root;
}
