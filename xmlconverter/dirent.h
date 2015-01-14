#pragma once

typedef struct DIR DIR;

struct dirent
{
	char * d_name;
};

DIR * opendir(const std::string & name);
int closedir(DIR *);
struct dirent * readdir(DIR *);
void rewinddir(DIR *);

typedef ptrdiff_t handle_type;

