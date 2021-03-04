#pragma once

#include "Delegates/DelegateCombinations.h"

class AMyDefaultPawn;

namespace UIDelegates {

	DECLARE_MULTICAST_DELEGATE_OneParam(FToggleMenuEvent, AMyDefaultPawn*);

	extern FToggleMenuEvent OnToggleMenu;

};