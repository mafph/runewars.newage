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

#include <algorithm>

#include "settings.h"
#include "actions.h"
#include "gametheme.h"
#include "gamesummarypart.h"

GameSummaryScreen::GameSummaryScreen() : JsonWindow("screen_game_summary.json", nullptr)
{
    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:category"));
    labels.back().text = _("Category");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:score1"));
    labels.back().text = _("Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:score2"));
    labels.back().text = _("Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:score3"));
    labels.back().text = _("Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:score4"));
    labels.back().text = _("Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:rank1"));
    labels.back().text = _("Rank");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:rank2"));
    labels.back().text = _("Rank");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:rank3"));
    labels.back().text = _("Rank");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:rank4"));
    labels.back().text = _("Rank");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:territory"));
    labels.back().text = _("Territory Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:summon"));
    labels.back().text = _("Summon Circle Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:unit"));
    labels.back().text = _("Unit Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:spell"));
    labels.back().text = _("Spell Point Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:land"));
    labels.back().text = _("Land Claim Score");

    labels.push_back(GameTheme::jsonTextInfo(jobject, "textinfo:total"));
    labels.back().text = _("Total Score");

    buttonNext = buttons.findIds("but_done");
    if(buttonNext)
        buttonNext->setAction(Action::ButtonDone);

    setVisible(true);
}

void GameSummaryScreen::renderWindow(void)
{
    JsonWindow::renderWindow();

    for(auto & label : labels)
	renderTextInfo(label);
}

bool GameSummaryScreen::keyPressEvent(const KeySym & key)
{
    switch(key.keycode())
    {
        case Key::SPACE:
            pushEventAction(Action::ButtonDone, this, nullptr);
            return true;

        default: break;
    }

    return false;
}

bool GameSummaryScreen::userEvent(int act, void* data)
{
    switch(act)
    {
        case Action::ButtonDone:
            setResultCode(Menu::GameExit);
            setVisible(false);
            break;

        default: break;
    }

    return true;
}
