#include "..\Utils\memory.h"
#include "CVector.h"
class IPrediction {
public:

	void SetLocalViewAngles(CVector& Angles)
	{
		mem::Call<void>(this, 13, &Angles);
	}
};