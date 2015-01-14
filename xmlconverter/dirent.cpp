#include <string>
#include <errno.h>
#include <io.h>
#include <stdlib.h>
#include "dirent.h"

struct DIR
{
	handle_type handle;
	struct _finddata_t info;
	struct dirent result;
	std::string name;
};

DIR * opendir(const std::string & name)
{
	DIR *dir = 0;

	if (!name.empty())
	{
		dir = new DIR;
		if (dir != nullptr)
		{
			dir->name = name;
			dir->name.append((name.find_last_of("/\\") == name.length() - 1 ? "*" : "/*"));

			dir->handle = (handle_type)_findfirst(dir->name.c_str(), &dir->info);
			if (dir->handle != -1)
			{
				dir->result.d_name = 0;
			}
			else
			{
				delete dir;
				dir = 0;
			}
		}
		else
		{
			delete dir;
			dir = 0;
			errno = ENOMEM;
		}
	}
	else
	{
		errno = EINVAL;
	}

	return dir;
}

int closedir(DIR *dir)
{
	int result = -1;

	if (dir)
	{
		if (dir->handle != -1)
		{
			result = _findclose(dir->handle);
		}
		delete dir;
	}

	if (result == -1) /* map all errors to EBADF */
	{
		errno = EBADF;
	}

	return result;
}

struct dirent *readdir(DIR *dir)
{
	struct dirent *result = 0;

	if (dir && dir->handle != -1)
	{
		if (!dir->result.d_name || _findnext(dir->handle, &dir->info) != -1)
		{
			result = &dir->result;
			result->d_name = dir->info.name;
		}
	}
	else
	{
		errno = EBADF;
	} 

	return result;
}

void rewinddir(DIR *dir)
{
	if (dir && dir->handle != -1)
	{
		_findclose(dir->handle);
		dir->handle = (handle_type)_findfirst(dir->name.c_str(), &dir->info);
		dir->result.d_name = 0;
	}
	else
	{
		errno = EBADF;
	}
}

