/***************************************************************************
 *   Copyright (C) 2020 by RuneWarsNA team <runewars.newage@gmail.com>     *
 *                                                                         *
 *   Part of the RuneWars: NewAge engine:                                  *
 *   https://github.com/AndreyBarmaley/runewars.newage                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "gametheme.h"
#include "runewars.h"
#include "settings.h"

namespace
{
    bool gameMusic = true;
    bool gameSound = true;
    bool gameAccel = true;
    bool gameFullscreen = false;
    bool guardianRulesSound = true;
}

bool Settings::read(void)
{
    JsonObject jo = GameTheme::jsonResource("config.json").toObject();

    if(jo.isValid())
    {
	gameMusic = jo.getBoolean("music", true);
	gameSound = jo.getBoolean("sound", true);
	gameFullscreen = jo.getBoolean("display:fullscreen", false);
	gameAccel = jo.getBoolean("display:accel", true);

	guardianRulesSound = jo.getBoolean("sound:guardianrules", true);
    }

    return true;
}

bool Settings::fullscreen(void)
{
    return gameFullscreen;
}

bool Settings::accel(void)
{
    return gameAccel;
}

bool Settings::music(void)
{
    return gameMusic;
}

bool Settings::sound(void)
{
    return gameSound;
}

bool Settings::soundGuardianRules(void)
{
    return guardianRulesSound;
}

//////////////////////////////////////////////////////
std::string Settings::fileSaveGame(void)
{
    return fileSave("game.sav");
}

std::string Settings::shareDir(void)
{
    return Systems::homeDirectory(Application::domain());
}

std::string Settings::fileSave(const std::string & file)
{
    return Systems::concatePath(Systems::homeDirectory(Application::domain()), file);
}

bool Settings::storeCache(void)
{
    return Systems::environment("RUNEWARS_STORE_CACHE");
}
