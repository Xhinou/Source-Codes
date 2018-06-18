// Steinbach, Aymeric - 1DAE08

#pragma once
//#include "Room.h"
#include <vector>
class Room;
class Avatar;
class SoundManager;

class RoomParser
{
public:
	RoomParser();

	Room* Parse( const std::string& fileName, Room*& pRoom, Avatar* pAvatar, SoundManager* pSoundManager ) const;
};