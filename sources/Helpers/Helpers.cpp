/*Author: Vingt-2
	BLAengine
*/
#include "Helpers.h"

int StringToInt(char* string,int base)
{
	int result = 0;
	for(int i = 0;string[i] != 0;i++)
	{
		int j;
		for(j = 0; string[j] !=0 ; j++);
		result += (int) powl(base,j-i-1) * (string[i] - '0');
	}
	return result;
}

//Works up to 10^start numbers
string DecimalToString(int integer,int start)
{
	string outputString = "";
	int i = start;
	bool zerosPassed = false;
	int dividend = (int) (integer / (int) powf(10, i));
	int remainder = integer - dividend * (int) powf(10, i);
	while(i>=0)
	{
		dividend = (int) (remainder / (int) powf(10, i));
		remainder = remainder - dividend * (int) powf(10, i);
		if(dividend != 0 || zerosPassed)
		{
			outputString += ('0' + dividend);
		}
		else if(dividend !=0 && !zerosPassed)
		{
			zerosPassed = true;
		}
		i--;
	}
	return outputString;
}
