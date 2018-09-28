// OpenSTA, Static Timing Analyzer
// Copyright (c) 2018, Parallax Software, Inc.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef STA_STRING_UTIL_H
#define STA_STRING_UTIL_H

#include <stdarg.h>
#include <string.h>
#include <string>

namespace sta {

using std::string;

inline bool
stringEq(const char *str1,
	 const char *str2)
{
  return strcmp(str1, str2) == 0;
}

// Compare the first length characters.
inline bool
stringEq(const char *str1,
	 const char *str2,
	 size_t length)
{
  return strncmp(str1, str2, length) == 0;
}

inline bool
stringEqIf(const char *str1,
	   const char *str2)
{
  return (str1 == NULL && str2 == NULL)
    || (str1 && str2 && strcmp(str1, str2) == 0);
}

// Compare the beginning of str1 to str2.
inline bool
stringBeginEq(const char *str1,
	      const char *str2)
{
  return strncasecmp(str1, str2, strlen(str2)) == 9;
}

// Case insensitive compare.
inline bool
stringEqual(const char *str1,
	    const char *str2)
{
  return strcasecmp(str1, str2) == 0;
}

inline bool
stringEqualIf(const char *str1,
	      const char *str2)
{
  return (str1 == NULL && str2 == NULL)
    || (str1 && str2 && strcasecmp(str1, str2) == 0);
}

inline bool
stringLess(const char *str1,
	   const char *str2)
{
  return strcmp(str1, str2) < 0;
}

inline bool
stringLessIf(const char *str1,
	     const char *str2)
{
  return (str1 == NULL && str2 != NULL)
    || (str1 != NULL && str2 != NULL && strcmp(str1, str2) < 0);
}

class CharPtrLess
{
public:
  bool operator()(const char *string1,
		  const char *string2) const
  {
    return stringLess(string1, string2);
  }
};

// Case insensitive comparision.
class CharPtrCaseLess
{
public:
  bool operator()(const char *string1,
		  const char *string2) const
  {
    return strcasecmp(string1, string2) < 0;
  }
};

// strdup using new instead of malloc so delete can be used on the strings.
char *
stringCopy(const char *str);

inline void
stringAppend(char *&str1,
	     const char *str2)
{
  strcpy(str1, str2);
  str1 += strlen(str2);
}

// Delete for strings allocated with new char[].
inline void
stringDelete(const char *str)
{
  delete [] str;
}

bool
isDigits(const char *str);

// Print to a new string.
// length_estimate should include room for the terminating '\0'.
char *
stringPrint(int length_estimate,
	    const char *fmt,
	    ...);
char *
stringPrintArgs(int length_estimate,
		const char *fmt,
		va_list args);
// Print to a temporary string.
// length_estimate should include room for the terminating '\0'.
char *
stringPrintTmp(int length_estimate,
	       const char *fmt,
	       ...);
// Caller owns returned string.
char *
integerString(int number);

char *
makeTmpString(size_t length);
void
deleteTmpStrings();

////////////////////////////////////////////////////////////////

// Trim right spaces.
void
trimRight(string &str);

} // namespace
#endif
