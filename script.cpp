#include "script.h"
#include "keyboard.h"
#include "mini/ini.h"

#pragma region VARIABLES
enum mapStates {
	original,
	big,
	zoomout,
	full,
	hidden
};
enum signalTypes {
	left,
	right,
	hazard
};
static DWORD CurrentWindowKey, PassengerWindowKey, DriverRearWindowKey, PassengerRearWindowKey, AllWindowsKey, HoodKey, TrunkKey, InteriorLightKey, ToggleRadioWheelKey, ToggleMobileRadioKey, ToggleMinimapKey, OpenDoorKey, SeatbeltKey, ShuffleSeatKey, LeftSignalKey, RightSignalKey, HazardsKey, RedLaserKey, GreenLaserKey, DropWeaponKey;
static int vehMinimapButton, vehRadioWheelButton, HazardsButton, LeftSignalButton, RightSignalButton, SeatbeltButton, InteriorLightButton, CurrentWindowButton, PassengerWindowButton, DriverRearWindowButton, PassengerRearWindowButton, AllWindowsButton, HoodButton, TrunkButton, OpenDoorButton, ShuffleSeatButton, MinimapButton, MobileRadioButton, RadioWheelButton, RedLaserButton, GreenLaserButton, DropWeaponButton;
bool MMoriginalEnabled, MMbigMapEnabled, MMzoomoutEnabled, MMfullEnabled, MMhiddenEnabled, enableVehicleControls, enableMinimapControls, enableMobileRadio, enablePhoneColor, LightOff = true;
bool leftSignalActive, rightSignalActive, hazardsActive, radioWheelDisabled, mobileRadio, beltedUp, isCurrentlyShuffling, window0down, window1down, window2down, window3down, AWindowDown, hoodOpen, trunkOpen, door0Open, door1Open, door2Open, door3Open, initialized = false;
int phoneColorIndex, MMsafetyVal = 0;
mapStates mapState;
int ControlsToDisable []  = { 15,47,52,58,85,115,174,261,14,46,54,74,101,103,104,119,175,262,344,355,356,27,42,172,18,21,70,73,105,114,120,132,136,137,141,154,176,258,264,337,345,353,354,357,45,57,80,140,177,263,23,49,53,56,75,144,145,0,244,28,36,86,113,350,351,352,7,26,29,50,79,93,121,37,38,68,89,117,133,152,346,44,55,69,76,90,102,118,134,153,347,358 };
static Hash laserHash = 2455710022;//COMPONENT_AT_AR_LASER
static Hash redLaserHash = 1073457922; //COMPONENT_AT_AR_LASER_RED
static Hash laserHashREH = 2248128277; //COMPONENT_AT_AR_LASER_REH
static Hash redLaserHashREH = 3620657966; //COMPONENT_AT_AR_LASER_RED_REH
static Hash piLaserHash = 1230280991; //COMPONENT_AT_PI_LASER
static Hash piLaserHashIR = 743205558; //COMPONENT_AT_PI_LASER_IR
mINI::INIFile file("ControlExtensionV.ini");
mINI::INIStructure ini;
#pragma endregion
#pragma region GENERAL FUNCTIONS
static void disableFuckingEverything() {
	for (int i : ControlsToDisable) {
		CONTROLS::DISABLE_CONTROL_ACTION(0, ControlsToDisable[i], 0);
	}
}
static Ped getPlayerPed() {
	return PLAYER::PLAYER_PED_ID();
}
static Weapon getPlayerCurrentWeapon() {
	return WEAPON::GET_SELECTED_PED_WEAPON(getPlayerPed());
}
void sendNotification(char* msg) {
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(msg);
	UI::_DRAW_NOTIFICATION(FALSE, FALSE);}
bool to_bool(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}
#pragma endregion
static void loadExternalSettings() {
	file.read(ini);
	phoneColorIndex = std::stoi(ini["MISC"]["CellPhoneColor"]);
	enableMinimapControls = to_bool(ini["FEATURES"]["enableMinimapControls"]);
	enableVehicleControls = to_bool(ini["FEATURES"]["enableVehicleControls"]);
	enableMobileRadio = to_bool(ini["FEATURES"]["enableMobileRadio"]);
	enablePhoneColor = to_bool(ini["FEATURES"]["enablePhoneColor"]);
	MMoriginalEnabled = to_bool(ini["MAP MODES"]["MMoriginalEnabled"]);
	MMbigMapEnabled = to_bool(ini["MAP MODES"]["MMbigMapEnabled"]);
	MMzoomoutEnabled = to_bool(ini["MAP MODES"]["MMzoomoutEnabled"]);
	MMfullEnabled = to_bool(ini["MAP MODES"]["MMfullEnabled"]);
	MMhiddenEnabled = to_bool(ini["MAP MODES"]["MMhiddenEnabled"]);
	CurrentWindowKey = std::stoi(ini["KEYBINDS"]["CurrentWindowKey"]);
	PassengerWindowKey = std::stoi(ini["KEYBINDS"]["PassengerWindowKey"]);
	DriverRearWindowKey = std::stoi(ini["KEYBINDS"]["DriverRearWindowKey"]);
	PassengerRearWindowKey = std::stoi(ini["KEYBINDS"]["PassengerRearWindowKey"]);
	AllWindowsKey = std::stoi(ini["KEYBINDS"]["AllWindowsKey"]);
	HoodKey = std::stoi(ini["KEYBINDS"]["HoodKey"]);
	TrunkKey = std::stoi(ini["KEYBINDS"]["TrunkKey"]);
	InteriorLightKey = std::stoi(ini["KEYBINDS"]["InteriorLightKey"]);
	ToggleRadioWheelKey = std::stoi(ini["KEYBINDS"]["ToggleRadioWheelKey"]);
	ToggleMobileRadioKey = std::stoi(ini["KEYBINDS"]["ToggleMobileRadioKey"]);
	ToggleMinimapKey = std::stoi(ini["KEYBINDS"]["ToggleMinimapKey"]);
	OpenDoorKey = std::stoi(ini["KEYBINDS"]["OpenDoorKey"]);
	SeatbeltKey = std::stoi(ini["KEYBINDS"]["SeatbeltKey"]);
	ShuffleSeatKey = std::stoi(ini["KEYBINDS"]["ShuffleSeatKey"]);
	LeftSignalKey = std::stoi(ini["KEYBINDS"]["LeftSignalKey"]);
	RightSignalKey = std::stoi(ini["KEYBINDS"]["RightSignalKey"]);
	HazardsKey = std::stoi(ini["KEYBINDS"]["HazardsKey"]);
	RedLaserKey = std::stoi(ini["KEYBINDS"]["RedLaserKey"]);
	GreenLaserKey = std::stoi(ini["KEYBINDS"]["GreenLaserKey"]);
	DropWeaponKey = std::stoi(ini["KEYBINDS"]["DropWeaponKey"]);
	vehMinimapButton = std::stoi(ini["CONTROLLER_VEHICLE"]["vehMinimapButton"]);
	vehRadioWheelButton = std::stoi(ini["CONTROLLER_VEHICLE"]["vehRadioWheelButton"]);
	HazardsButton = std::stoi(ini["CONTROLLER_VEHICLE"]["HazardsButton"]);
	LeftSignalButton = std::stoi(ini["CONTROLLER_VEHICLE"]["LeftSignalButton"]);
	RightSignalButton = std::stoi(ini["CONTROLLER_VEHICLE"]["RightSignalButton"]);
	SeatbeltButton = std::stoi(ini["CONTROLLER_VEHICLE"]["SeatbeltButton"]);
	InteriorLightButton = std::stoi(ini["CONTROLLER_VEHICLE"]["InteriorLightButton"]);
	CurrentWindowButton = std::stoi(ini["CONTROLLER_VEHICLE"]["CurrentWindowButton"]);
	PassengerWindowButton = std::stoi(ini["CONTROLLER_VEHICLE"]["PassengerWindowButton"]);
	DriverRearWindowButton = std::stoi(ini["CONTROLLER_VEHICLE"]["DriverRearWindowButton"]);
	PassengerRearWindowButton = std::stoi(ini["CONTROLLER_VEHICLE"]["PassengerRearWindowButton"]);
	AllWindowsButton = std::stoi(ini["CONTROLLER_VEHICLE"]["AllWindowsButton"]);
	HoodButton = std::stoi(ini["CONTROLLER_VEHICLE"]["HoodButton"]);
	TrunkButton = std::stoi(ini["CONTROLLER_VEHICLE"]["TrunkButton"]);
	OpenDoorButton = std::stoi(ini["CONTROLLER_VEHICLE"]["OpenDoorButton"]);
	ShuffleSeatButton = std::stoi(ini["CONTROLLER_VEHICLE"]["ShuffleSeatButton"]);
	MinimapButton = std::stoi(ini["CONTROLLER_ON_FOOT"]["MinimapButton"]);
	MobileRadioButton = std::stoi(ini["CONTROLLER_ON_FOOT"]["MobileRadioButton"]);
	RadioWheelButton = std::stoi(ini["CONTROLLER_ON_FOOT"]["RadioWheelButton"]);
	RedLaserButton = std::stoi(ini["CONTROLLER_ON_FOOT"]["RedLaserButton"]);
	GreenLaserButton = std::stoi(ini["CONTROLLER_ON_FOOT"]["GreenLaserButton"]);
	DropWeaponButton = std::stoi(ini["CONTROLLER_ON_FOOT"]["DropWeaponButton"]);
}
static void checkForKeys() {
	//same thing as OnKeyDown
}
static void checkForButtons() {
	//check for controller keys (this should have been its own function in the first place lol
}
static void updateFeatures() {
	//the old on tick event
}
#pragma region MINIMAP FUNCTIONS
static void incrementMapState() {
	switch (mapState)
	{
	case original:
		mapState = big;
		break;
	case big:
		mapState = zoomout;
		break;
	case zoomout:
		mapState = full;
		break;
	case full:
		mapState = hidden;
		break;
	case hidden:
		mapState = original;
		break;
	default:
		break;
	}
}
static void setMinimapMode(mapStates mapState) {
	switch (mapState) {
	case original:
		UI::_SET_RADAR_BIGMAP_ENABLED(false, false);
		UI::DISPLAY_RADAR(true);
		break;
	case big:
		UI::_SET_RADAR_BIGMAP_ENABLED(true, false);
		break;
	case zoomout:
		UI::_SET_RADAR_BIGMAP_ENABLED(true, false);
		UI::SET_RADAR_ZOOM(6000);
		break;
	case full:
		UI::SET_RADAR_ZOOM(0);
		UI::_SET_RADAR_BIGMAP_ENABLED(true, true);
		break;
	case hidden:
		UI::DISPLAY_RADAR(false);
		break;
	default:
		break;
	}
}
static void minimapSafetyCheck() {
	MMsafetyVal = MMoriginalEnabled + MMbigMapEnabled + MMzoomoutEnabled + MMfullEnabled + MMhiddenEnabled;
	if (MMsafetyVal >= 1) {
		if (MMoriginalEnabled) { setMinimapMode(original); }
		else if (MMbigMapEnabled) { setMinimapMode(big); }
		else if (MMzoomoutEnabled) { setMinimapMode(zoomout); }
		else if (MMfullEnabled) { setMinimapMode(full); }
		else if (MMhiddenEnabled) { setMinimapMode(hidden); }
	}
}
static void cycleMinimapState() {
	incrementMapState();
	switch (mapState)
	{
	case original:
		if (MMoriginalEnabled) { setMinimapMode(mapState); }
		else { cycleMinimapState(); }
		break;
	case big:
		if (MMbigMapEnabled) { setMinimapMode(mapState); }
		else { cycleMinimapState(); }
		break;
	case zoomout:
		if (MMzoomoutEnabled) { setMinimapMode(mapState); }
		else { cycleMinimapState(); }
		break;
	case full:
		if (MMfullEnabled) { setMinimapMode(mapState); }
		else { cycleMinimapState(); }
		break;
	case hidden:
		if (MMhiddenEnabled) { setMinimapMode(mapState); }
		else { cycleMinimapState(); }
		break;
	default:
		break;
	}
}

#pragma endregion
#pragma region VEHICLE FUNCTIONS
static Vehicle getPlayerVehicle() {
	if (PED::IS_PED_IN_ANY_VEHICLE(getPlayerPed(), 0)) {
		return PED::GET_VEHICLE_PED_IS_USING(getPlayerPed());
	}
	return NULL;
}
static bool isPlayerInThisSeat(int seatIndex) { 
	if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(getPlayerVehicle(), seatIndex) == getPlayerPed()) {
		return true;
	}
	return false;
}
static int getPlayerSeat() {
	if (isPlayerInThisSeat(-1)) { return -1; }
	else if (isPlayerInThisSeat(0)) { return 0; }//front passenger
	else if (isPlayerInThisSeat(1)) { return 1; }//driver side rear
	else if (isPlayerInThisSeat(2)) { return 2; }//passenger side rear
	else if (isPlayerInThisSeat(3)) { return 3; }//extra seats for compatibility up to the 10 seat Cargobob
	else if (isPlayerInThisSeat(4)) { return 4; }
	else if (isPlayerInThisSeat(5)) { return 5; }
	else if (isPlayerInThisSeat(6)) { return 6; }
	else if (isPlayerInThisSeat(7)) { return 7; }
	else if (isPlayerInThisSeat(8)) { return 8; }
	return -2;
}
static void toggleCurrentWindow() {
	int seatIndex = getPlayerSeat();
	switch (seatIndex) {
	case -2:
		//seat index return failed, do nothing
		break;
	case -1://driver
		if (window0down == false) {
			AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
			VEHICLE::ROLL_DOWN_WINDOW(getPlayerVehicle(), 0);
			window0down = true;
		}
		else {
			AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
			VEHICLE::ROLL_UP_WINDOW(getPlayerVehicle(), 0);
			window0down = false;
		}
		break;
	case 0://passenger
		if (window1down == false) {
			AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
			VEHICLE::ROLL_DOWN_WINDOW(getPlayerVehicle(), 1);
			window1down = true;
		}
		else {
			AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
			VEHICLE::ROLL_UP_WINDOW(getPlayerVehicle(), 1);
			window1down = false;
		}
		break;
	case 1://driver rear
		if (window2down == false) {
			AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
			VEHICLE::ROLL_DOWN_WINDOW(getPlayerVehicle(), 2);
			window2down = true;
		}
		else {
			AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
			VEHICLE::ROLL_UP_WINDOW(getPlayerVehicle(), 2);
			window2down = false;
		}
		break;
	case 2://passenger rear
		if (window3down == false) {
			AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
			VEHICLE::ROLL_DOWN_WINDOW(getPlayerVehicle(), 3);
			window3down = true;
		}
		else {
			AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
			VEHICLE::ROLL_UP_WINDOW(getPlayerVehicle(), 3);
			window3down = false;
		}
		break;
	}
}
static void driverWindowAccess(int whichWindow) {
	if (isPlayerInThisSeat(-1)) {//Make sure player is Driver
		switch (whichWindow) {
		case 1://passenger
			if (window1down == false) {
				AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
				VEHICLE::ROLL_DOWN_WINDOW(getPlayerVehicle(), 1);
				window1down = true;
			}
			else {
				AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
				VEHICLE::ROLL_UP_WINDOW(getPlayerVehicle(), 1);
				window1down = false;
			}
			break;
		case 2://driver rear
			if (window2down == false) {
				AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
				VEHICLE::ROLL_DOWN_WINDOW(getPlayerVehicle(), 2);
				window2down = true;
			}
			else {
				AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
				VEHICLE::ROLL_UP_WINDOW(getPlayerVehicle(), 2);
				window2down = false;
			}
			break;
		case 3://passenger rear
			if (window3down == false) {
				AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
				VEHICLE::ROLL_DOWN_WINDOW(getPlayerVehicle(), 3);
				window3down = true;
			}
			else {
				AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
				VEHICLE::ROLL_UP_WINDOW(getPlayerVehicle(), 3);
				window3down = false;
			}
			break;
		}
	}
}
static void toggleAllWindows() {
	if (isPlayerInThisSeat(-1)) {//ensure player is in drivers seat
		if (!AWindowDown) {
			VEHICLE::ROLL_DOWN_WINDOWS(getPlayerVehicle());
			window0down, window1down, window2down, window3down = true;
			AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
		}
		else {
			for (int i = 0; i < 8; i++) {VEHICLE::ROLL_UP_WINDOW(getPlayerVehicle(), i);}//roll up all windows because rockstar doesn't have a function for that ffs
			window0down, window1down, window2down, window3down = false;
			AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
		}
		AWindowDown = !AWindowDown;
	}
}
static void toggleInteriorLight() {
	switch (LightOff) {
	case false://light is on, turn it off
		AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
		VEHICLE::SET_VEHICLE_INTERIORLIGHT(getPlayerVehicle(), false);
		LightOff = true;
		break;

	case true://light is off, turn it on
		AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
		VEHICLE::SET_VEHICLE_INTERIORLIGHT(getPlayerVehicle(), true);
		LightOff = false;
		break;
	}
}
static void toggleHood() {
	if (isPlayerInThisSeat(-1)) {//ensure player is driver
		if (hoodOpen) { VEHICLE::SET_VEHICLE_DOOR_SHUT(getPlayerVehicle(), 4, false); }
		else { VEHICLE::SET_VEHICLE_DOOR_OPEN(getPlayerVehicle(), 4, false, false); }
		hoodOpen = !hoodOpen;
	}
}
static void toggleTrunk() {
	if (isPlayerInThisSeat(-1)) {//ensure player is driver
		if (trunkOpen) { VEHICLE::SET_VEHICLE_DOOR_SHUT(getPlayerVehicle(), 5, false); }
		else { VEHICLE::SET_VEHICLE_DOOR_OPEN(getPlayerVehicle(), 5, false, false); }
		trunkOpen = !trunkOpen;
	}
}
static void openLocalDoor() {
	int seatIndex = getPlayerSeat();
	switch (seatIndex) {
	case -2://Seat index return failed, do nothing
		break;
	case -1://driver
		if (door0Open == false) {
			VEHICLE::SET_VEHICLE_DOOR_OPEN(getPlayerVehicle(), 0, false, false);
			door0Open = true;
		}
		else {
			VEHICLE::SET_VEHICLE_DOOR_SHUT(getPlayerVehicle(), 0, false);
			door0Open = false;
		}
		break;
	case 0://passenger
		if (door1Open == false) {
			VEHICLE::SET_VEHICLE_DOOR_OPEN(getPlayerVehicle(), 1, false, false);
			door1Open = true;
		}
		else {
			VEHICLE::SET_VEHICLE_DOOR_SHUT(getPlayerVehicle(), 1, false);
			door1Open = false;
		}
		break;
	case 1://driver rear
		if (door2Open == false) {
			VEHICLE::SET_VEHICLE_DOOR_OPEN(getPlayerVehicle(), 2, false, false);
			door2Open = true;
		}
		else {
			VEHICLE::SET_VEHICLE_DOOR_SHUT(getPlayerVehicle(), 2, false);
			door2Open = false;
		}
		break;
	case 2://passenger rear
		if (door3Open == false) {
			VEHICLE::SET_VEHICLE_DOOR_OPEN(getPlayerVehicle(), 3, false, false);
			door3Open = true;
		}
		else {
			VEHICLE::SET_VEHICLE_DOOR_SHUT(getPlayerVehicle(), 3, false);
			door3Open = false;
		}
		break;
	}
}
static void shuffleToNextSeat() {
	if (!AI::GET_IS_TASK_ACTIVE(getPlayerPed(), 165) && !beltedUp) {//if player is not belted in AND isnt already playing this task
		AI::TASK_SHUFFLE_TO_NEXT_VEHICLE_SEAT(getPlayerPed(), getPlayerVehicle());
	}
		
}
static void togglePlayerSeatbelt() {
	if (!beltedUp) {
		beltedUp = true;
		AUDIO::PLAY_SOUND(-1, "YES", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
		PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(getPlayerPed(), 1);
		PED::SET_PED_CONFIG_FLAG(getPlayerPed(), 32, false);
		PED::SET_PED_CAN_BE_DRAGGED_OUT(getPlayerPed(), false);
		AI::TASK_PLAY_ANIM(getPlayerPed(), "swimming@scuba", "scuba_tank_off", 4.0, -2.0, -1, 48, 0, false, false, false);
	}
	else {
		AUDIO::PLAY_SOUND(-1, "NO", "HUD_FRONTEND_DEFAULT_SOUNDSET", false, 0, true);
		PED::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(getPlayerPed(), 0);
		PED::SET_PED_CONFIG_FLAG(getPlayerPed(), 32, true);
		PED::SET_PED_CAN_BE_DRAGGED_OUT(getPlayerPed(), true);
		AI::TASK_PLAY_ANIM(getPlayerPed(), "swimming@scuba", "scuba_tank_off", 4.0, -2.0, -1, 48, 0, false, false, false);
		beltedUp = false;
	}
}
static void toggleTurnSignals(signalTypes signalType) {
	if (isPlayerInThisSeat(-1)) {//ensure player is in drivers seat
		switch (signalType) {
		case left:
			if (!hazardsActive) {
				leftSignalActive = !leftSignalActive;
				VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(getPlayerVehicle(), 1, leftSignalActive);
				if (leftSignalActive) { VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(getPlayerVehicle(), 0, false); rightSignalActive = false; }
			}
			break;
		case right:
			if (!hazardsActive) {
				rightSignalActive = !rightSignalActive;
				VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(getPlayerVehicle(), 0, rightSignalActive);
				if (rightSignalActive) { VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(getPlayerVehicle(), 1, false); leftSignalActive = false; }
			}
			break;
		case hazard:
			if (!hazardsActive) {
				leftSignalActive, rightSignalActive = false;
				hazardsActive = true;
			}
			else {
				hazardsActive = false;
			}
			VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(getPlayerVehicle(), 1, hazardsActive);
			VEHICLE::SET_VEHICLE_INDICATOR_LIGHTS(getPlayerVehicle(), 0, hazardsActive);
			break;
		default:
			break;
		}
	}
}
#pragma endregion
#pragma region RADIO FUNCTIONS
static void togglePlayerMobileRadio() {
	if (getPlayerVehicle() == NULL) {
		if (mobileRadio) {
			AUDIO::SET_MOBILE_PHONE_RADIO_STATE(false);
			AUDIO::SET_AUDIO_FLAG("MobileRadioInGame", 0);
			AUDIO::SET_AUDIO_FLAG("AllowRadioDuringSwitch", 0);
			sendNotification("Mobile Radio Off");
			mobileRadio = false;
			radioWheelDisabled = false;
		}
		else {
			AUDIO::SET_MOBILE_PHONE_RADIO_STATE(true);
			AUDIO::SET_AUDIO_FLAG("MobileRadioInGame", 1);
			AUDIO::SET_AUDIO_FLAG("AllowRadioDuringSwitch", 1);
			sendNotification("Mobile Radio On");
			mobileRadio = true;
		}
	}
}
static void toggleRadioWheel() {
	if (radioWheelDisabled) {
		radioWheelDisabled = false;
		sendNotification("Radio Wheel Enabled");
	}
	else {
		radioWheelDisabled = true;
		sendNotification("Radio Wheel Disabled");
	}
}
#pragma endregion
#pragma region WEAPON FUNCTIONS
static void toggleLaser(bool laserColor) {
	if (laserColor) {//green
		if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(getPlayerPed(), getPlayerCurrentWeapon(), 2455710022)) {//current weapon already has laser1
			WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(getPlayerPed(), getPlayerCurrentWeapon(), 2455710022);//remove laser1
		}
		else {//current weapon does NOT have laser1
			WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(getPlayerPed(), getPlayerCurrentWeapon(), 2455710022);//give laser1
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(getPlayerPed(), getPlayerCurrentWeapon(), 2248128277)) {//current weapon already has laser2
			WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(getPlayerPed(), getPlayerCurrentWeapon(), 2248128277);//remove laser2
		}
		else {//current weapon does NOT have laser2
			WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(getPlayerPed(), getPlayerCurrentWeapon(), 2248128277);//give laser2
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(getPlayerPed(), getPlayerCurrentWeapon(), 2248128277)) {//current weapon already has laser3
			WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(getPlayerPed(), getPlayerCurrentWeapon(), 2248128277);//remove laser3
		}
		else {//current weapon does NOT have laser3
			WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(getPlayerPed(), getPlayerCurrentWeapon(), 2248128277);//give laser3
		}
	}else//red
	{
		if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(getPlayerPed(), getPlayerCurrentWeapon(), 1073457922)) {//current weapon already has laser1
			WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(getPlayerPed(), getPlayerCurrentWeapon(), 1073457922);//remove laser1
		}
		else {//current weapon does NOT have laser1
			WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(getPlayerPed(), getPlayerCurrentWeapon(), 1073457922);//give laser1
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(getPlayerPed(), getPlayerCurrentWeapon(), 3620657966)) {//current weapon already has laser2
			WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(getPlayerPed(), getPlayerCurrentWeapon(), 3620657966);//remove laser2
		}
		else {//current weapon does NOT have laser2
			WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(getPlayerPed(), getPlayerCurrentWeapon(), 3620657966);//give laser2
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(getPlayerPed(), getPlayerCurrentWeapon(), 743205558)) {//current weapon already has laser3
			WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(getPlayerPed(), getPlayerCurrentWeapon(), 743205558);//remove laser3
		}
		else {//current weapon does NOT have laser3
			WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(getPlayerPed(), getPlayerCurrentWeapon(), 743205558);//give laser3
		}
	}
}
static void dropWeapon() {
	if (WEAPON::IS_PED_ARMED(getPlayerPed(), 7) && !ENTITY::IS_ENTITY_PLAYING_ANIM(getPlayerPed(),"weapons@projectile@", "drop", 3)) {
		AI::TASK_PLAY_ANIM(getPlayerPed(), "weapons@projectile@", "drop", 2.0, -2.0, -1, 48, 0, false, false, false);
		WEAPON::SET_PED_DROPS_INVENTORY_WEAPON(getPlayerPed(), getPlayerCurrentWeapon(), 0, 0, 0, 0);
	}
}
#pragma endregion


int main() {
	loadExternalSettings();
	minimapSafetyCheck();
	STREAMING::REQUEST_ANIM_DICT("weapons@projectile@");
	STREAMING::REQUEST_ANIM_DICT("swimming@scuba");
	while (true)
	{
		updateFeatures();
		checkForKeys();
		checkForButtons();
		WAIT(0);
	}

	return 0;
}
void ScriptMain() {
	srand(GetTickCount());
	throw main();
}