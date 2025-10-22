/***************************************************************************
 *   Copyright by Johannes Herwig (NanoFocus AG) 2007                      *
 *   Copyright by Georg Wiora (NanoFocus AG) 2007                          *
 *                                                                         *
 *   This file is part of the openGPS (R)[TM] software library.            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License (LGPL)    *
 *   as published by the Free Software Foundation; either version 3 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the files "licence_LGPL-3.0.txt" and                   *
 *   "licence_GPL-3.0.txt".                                                *
 *                                                                         *
 *   openGPS is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   The name "openGPS" and the logo are registered as                     *
 *   European trade mark No. 006178354 for                                 *
 *   Physikalisch Technische Bundesanstalt (PTB)                           *
 *   http://www.ptb.de/                                                    *
 *                                                                         *
 *   More information about openGPS can be found at                        *
 *   http://www.opengps.eu/                                                *
 ***************************************************************************/

#ifndef _WIN32
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#include "linux_environment.hxx"

#include <opengps/cxx/string.hxx>

#include <sstream>

#ifdef _UNICODE

namespace OpenGPS
{
	typedef std::wostringstream OutStringStream;
}

#else

namespace OpenGPS
{
	typedef std::ostringstream OutStringStream;
}

#endif

using namespace OpenGPS;


int copyFile(const char* src, const char* dst)
{
	FILE* in_fd = nullptr, * out_fd = nullptr;
	size_t n_chars;
	const size_t BUFSIZE = 4096;
	char buf[BUFSIZE];


	// open files
	if ((in_fd = fopen(src, "r")) == nullptr)
	{
		return -1;
	}


	if ((out_fd = fopen(dst, "w")) == nullptr)
	{
		return -1;
	}

	// copy files
	while ((n_chars = fread(buf, 1, BUFSIZE, in_fd)) > 0)
	{
		if (fwrite(buf, 1, n_chars, out_fd) != n_chars)
		{
			// write error
			return -1;
		}
	}


	// close files
	if (fclose(in_fd) == -1 || fclose(out_fd) == -1)
	{
		// error closing file
		return -1;
	}

	return 0;
}

LinuxEnvironment::LinuxEnvironment()
	: Environment()
{
}

LinuxEnvironment::~LinuxEnvironment()
{
}

bool LinuxEnvironment::m_InitRandom = true;

OGPS_Character LinuxEnvironment::GetDirectorySeparator() const
{
	return _T('/');
}

OGPS_Character LinuxEnvironment::GetAltDirectorySeparator() const
{
	return _T('/');
}

bool LinuxEnvironment::GetPathName(const String& path, String& clean_path) const
{
	assert(path.length() > 0);
	assert(path.c_str() != clean_path.c_str());

	/* See for reference: http://msdn2.microsoft.com/en-us/library/aa365247(VS.85).aspx */

	/* TODO: This is currently not verified:
	 * Do not use the following reserved device names for the name of a file: CON, PRN, AUX, NUL, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, LPT1, LPT2, LPT3, LPT4, LPT5, LPT6, LPT7, LPT8, and LPT9. Also avoid these names followed by an extension, for example, NUL.tx7.
	 */

	const size_t length = path.size();

	clean_path.resize(length);
	clean_path.erase();

	const OGPS_Character separator = GetDirectorySeparator();

	bool hasDroppedChars = false;
	bool hasPathSeparator = false;

	/* Use any character in the current code page for a name, including Unicode characters, except characters in the range of zero (0) through 31, or any character that the file system does not allow. A name can contain characters in the extended character set (128�255). However, it cannot contain the following reserved characters: < > : " / \ | ? *
	*/
	for (size_t index = 0; index < length; ++index)
	{
		const OGPS_Character c = path.c_str()[index];

		if (c > 31 &&
			c != _T('<') &&
			c != _T('>') &&
			/* c != _T(':') && TODO: This check is currently omitted, because ':' is allowed in volume names... */
			c != _T('\"') &&
			c != _T('|') &&
			/* c != _T('?') &&  && TODO: This check is currently omitted, because '?' is allowed in UNC names... */
			c != _T('*'))
		{
			if (c == separator || c == GetAltDirectorySeparator())
			{
				if (hasPathSeparator)
				{
					/* Support UNC names, namely: \\<server>\<share> */
					if (clean_path.size() == 0)
					{
						clean_path.append(&separator, 1);
					}

					continue;
				}

				hasPathSeparator = true;
				continue;
			}

			if (hasPathSeparator)
			{
				clean_path.append(&separator, 1);
				hasPathSeparator = false;
			}

			clean_path.append(&c, 1);
			continue;
		}

		hasDroppedChars = true;
	}

	/* Do not end a file or directory name with a trailing space or a period. Although the underlying file system may support such names, the operating system does not. You can start a name with a period (.).
	 */
	const size_t length2 = clean_path.size();
	for (size_t index2 = length2 - 1; index2 > 0; --index2)
	{
		const OGPS_Character c2 = clean_path.c_str()[index2];

		if (c2 == _T(' ') || c2 == _T('.'))
		{
			clean_path.resize(index2);
			hasDroppedChars = true;
			continue;
		}

		break;
	}

	return !hasDroppedChars;
}

String LinuxEnvironment::GetFileName(const String& path) const
{
	String pattern;
	pattern.append(1, GetDirectorySeparator());
	pattern.append(1, GetAltDirectorySeparator());

	const size_t found = path.find_last_of(pattern);

	if (found != String::npos)
	{
		return path.substr(found + 1);
	}

	return path;
}

String LinuxEnvironment::ConcatPathes(const String& path1, const String& path2) const
{
	assert(path1.length() > 0 || path2.length() > 0);

	// TODO: see .NET implementation

	if (path1.length() == 0)
	{
		return path2;
	}

	if (path2.length() == 0)
	{
		return path1;
	}

	bool path2StartsWithSeparator = (path2[0] == GetDirectorySeparator() || path2[0] == GetAltDirectorySeparator());

	String path = path1;

	if (path1[path1.length() - 1] != GetDirectorySeparator() && path1[path1.length() - 1] != GetAltDirectorySeparator())
	{
		if (!path2StartsWithSeparator)
		{
			path += GetDirectorySeparator();
		}
	}
	else
	{
		if (path2StartsWithSeparator)
		{
			path += path2.substr(1, path2.length() - 1);
			return path;
		}
	}

	path += path2;

	return path;
}

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES          (unsigned long)-1
#endif /* INVALID_FILE_ATTRIBUTES */

bool LinuxEnvironment::PathExists(const String& file) const
{
	assert(file.length() > 0);

	ResetLastErrorCode();

	struct stat dwAttr;
	String tempFile(file.c_str());
	if (stat(tempFile.ToChar(), &dwAttr)) //GetFileAttributes(file.c_str());
		//   if (dwAttr == INVALID_FILE_ATTRIBUTES)
	{
		const unsigned long dwError = errno;
		if (dwError == ENOENT)
		{
			// file not found
			return false;
		}
		/*
			  else if (dwError == ERROR_PATH_NOT_FOUND)
			  {
				 // path not found
				 return false;
			  }
		*/
		else if (dwError == EACCES)
		{
			// file or directory exists, but access is denied
			return true;
		}
		//else
		{
			// some other error has occured
			assert(false);
			return false;
		}
	}
	//else
	{
		/*
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
		// this is a directory
		}
		else
		{
		// this is an ordinary file
		}
		*/
		return true;
	}
}

bool LinuxEnvironment::RemoveFile(const String& file) const
{
	assert(file.length() > 0);

	ResetLastErrorCode();

	//   return (DeleteFile(file.c_str()) != 0);
	String tempFile(file.c_str());
	return (remove(tempFile.ToChar()) != 0);
}

#define GETRANDOM(min, max) ((rand()%(int)(((max) + 1)-(min)))+ (min))

String LinuxEnvironment::GetUniqueName() const
{
	if (m_InitRandom)
	{
		srand((unsigned int)time(nullptr));
		m_InitRandom = false;
	}

	// random positive number with max. 8 digits
	int randNum = GETRANDOM(0, 10000000);

	OutStringStream os;
	os << randNum;

	return os.str();
}

bool LinuxEnvironment::CreateDir(const String& path) const
{
	assert(path.length() > 0);

	ResetLastErrorCode();

	String tempPath(path.c_str());
	//   umask(0);
	//   return (mkdir(tempPath.ToChar(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
	return (mkdir(tempPath.ToChar(), 0755) == 0);
}

bool LinuxEnvironment::RemoveDir(const String& path) const
{
	DIR* dir;
	struct dirent* entry;
	assert(path.length() > 0);

	ResetLastErrorCode();

	String tempPath(path.c_str());
	dir = opendir(tempPath.ToChar());
	if (dir == nullptr)
		return 0;
	while ((entry = readdir(dir)) != nullptr)
	{
		if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
			continue;
		else if (entry->d_type == DT_DIR)
		{
			String subdir;
			subdir.FromChar(entry->d_name);
			RemoveDir(subdir);
		}
		else
		{
			remove(entry->d_name);
		}
	}
	closedir(dir);

	remove(tempPath.ToChar());

	return (errno != 0);
}

String LinuxEnvironment::GetTempDir() const
{
	ResetLastErrorCode();

	char* tmpPath = getenv("TMPDIR");

	if (tmpPath && strlen(tmpPath) != 0)
	{
		String tempPath;
		tempPath.FromChar(tmpPath);
		return tempPath;
	}
	else
	{
		DIR* dir = opendir("/tmp");
		if (dir != nullptr)
		{
			closedir(dir);
			return _T("/tmp/");
		}
	}

	assert(false);

	return _T("");
}

bool LinuxEnvironment::RenameFile(const String& src, const String& dst) const
{
	ResetLastErrorCode();
	int ret;

	String tempSrc(src.c_str());
	String tempDst(dst.c_str());
	//   ret = copy(tempSrc.ToChar(), tempDst.ToChar());
	ret = copyFile(tempSrc.ToChar(), tempDst.ToChar());
	ret += unlink(tempSrc.ToChar());

	return !ret;
	//   return (MoveFileEx(src.c_str(), dst.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0);
}

bool LinuxEnvironment::GetVariable(const String& varName, String& value) const
{
	ResetLastErrorCode();

	String tempVarName(varName.c_str());
	char* varbuf = getenv(tempVarName.ToChar());

	if (!varbuf)
	{
		assert(varbuf == nullptr);

		return false;
	}

	value.FromChar(varbuf);

	return true;
}

String LinuxEnvironment::GetLastErrorMessage() const
{
	String message;
	message.FromChar(strerror(errno));

	return message;
}

void LinuxEnvironment::ResetLastErrorCode() const
{
	errno = 0;
}

std::unique_ptr<Environment> Environment::CreateInstance()
{
	return std::make_unique<LinuxEnvironment>();
}

#endif
