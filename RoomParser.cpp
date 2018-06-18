// Steinbach, Aymeric - 1DAE08

#include "stdafx.h"
#include "RoomParser.h"
#include "Room.h"
#include "Avatar.h"
#include "Tile.h"
#include "Door.h"
#include "Pickable.h"
#include "Goal.h"
#include "Enemy_Spiky.h"
#include <fstream>
#include <sstream>

RoomParser::RoomParser()
{
}

Room* RoomParser::Parse( const std::string& fileName, Room*& pRoom, Avatar* pAvatar, SoundManager* pSoundManager ) const
{
	using namespace std;

	ifstream file;
	file.open( fileName );

	if( file.is_open() )
	{
		string line{};
		while( getline( file, line ) && line != "end" )
		{
			string
				roomTag{ "room" },
				tileTag{ "tiles" },
				enemyTag{ "enemies" },
				doorTag{ "doors" },
				pickTag{ "pickable" },
				endTag{ "end" };

			if( line.find( roomTag ) != string::npos )
			{
				stringstream roomInfo{ line.substr( roomTag.size() ) };
				string tileset, background;
				unsigned int cols, rows;
				roomInfo >> tileset >> background >> cols >> rows;
				if (pRoom == nullptr )
					pRoom = new Room{ pAvatar, tileset, background, cols, rows };
			}
			if( pRoom != nullptr )
			{
				if( line.find( tileTag ) != string::npos )
				{
					stringstream ss{ line.substr( tileTag.size() ) };
					int count{};
					ss >> count;
					for( int i{}; i < count; ++i )
					{
						getline( file, line );
						stringstream tileInfo{ line };
						Index posIdx, texIdx;
						int hardness;
						tileInfo >> posIdx.i >> posIdx.j >> texIdx.i >> texIdx.j >> hardness;
						pRoom->AddTile( new Tile{ posIdx, texIdx, hardness } );
					}
				}
				else if( line.find( enemyTag ) != string::npos )
				{
					stringstream ss{ line.substr( enemyTag.size() ) };
					int count{};
					ss >> count;
					for( int i{}; i < count; ++i )
					{
						getline( file, line );
						stringstream enemyInfo{ line };
						int id;
						Index startPos;
						enemyInfo >> id >> startPos.i >> startPos.j;
						pRoom->AddEntity( new Enemy_Spiky{ pRoom, pAvatar, pSoundManager, "wl3_spritesheet_enemy_spiky.png", Index{4, 5}, Vector2f{2.5f, 0}, 20, 15, startPos } );
					}
				}
				else if( line.find( doorTag ) != string::npos )
				{
					stringstream ss{ line.substr( doorTag.size() ) };
					int count{};
					ss >> count;
					for( int i{}; i < count; ++i )
					{
						getline( file, line );
						stringstream doorInfo{ line };
						Index posIdx;
						int roomID;
						Index startPos;
						doorInfo >> posIdx.i >> posIdx.j >> roomID >> startPos.i >> startPos.j;
						pRoom->AddDoor( new Door( posIdx, roomID, startPos ) );
					}
				}
				else if( line.find( pickTag ) != string::npos )
				{
					stringstream ss{ line.substr( pickTag.size() ) };
					int count{};
					ss >> count;
					for( int i{}; i < count; ++i )
					{
						getline( file, line );
						stringstream pickInfo{ line };
						int itemID, nrFrames;
						string spriteFileName;
						Index spriteRowsCols;
						Vector2f colVect;
						float colWidth, colHeight;
						Index startPos;
						pickInfo >> itemID >> spriteFileName >> spriteRowsCols.i >> spriteRowsCols.j >> colVect.x >> colVect.y >> colWidth >> colHeight >> startPos.i >> startPos.j >> nrFrames;
						if( !pAvatar->HasItem( itemID ) )
						{
							pRoom->AddEntity( new Pickable{ pRoom, pAvatar, pSoundManager, itemID, spriteFileName, spriteRowsCols, colVect, colWidth, colHeight, startPos, nrFrames } );
						}
					}
				}
				else if( line.find( endTag ) != string::npos )
				{
					stringstream endInfo{ line.substr( endTag.size() ) };
					string spriteFileName;
					Index spriteRowsCols, startPos;
					Vector2f colVect;
					float colWidth, colHeight;
					int nrFrames;
					endInfo >> spriteFileName >> spriteRowsCols.i >> spriteRowsCols.j >> colVect.x >> colVect.y >> colWidth >> colHeight >> startPos.i >> startPos.j >> nrFrames;
					pRoom->SetGoal( new Goal{ pRoom, pSoundManager, spriteFileName, spriteRowsCols, colVect, colWidth, colHeight, startPos, nrFrames } );
				}
			}	
		}
		file.close();
	}
	return pRoom;
}