#include "string.h"
#include <engine/common/base_types.h>

namespace data_files{

	struct _file_version
	{
		char format[6];
		float ver;

		int cmp(_file_version* in)
		{
			if(in->ver == ver && !strcmp(format, in->format))return true;
			return false;
		}
	};
};