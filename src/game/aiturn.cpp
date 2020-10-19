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

#include <set>
#include "aiturn.h"

namespace GameData
{
    extern LocalPlayers gamers;

    LocalPlayer &	playerOfAvatar(const Avatar &);
    LocalPlayer &	playerOfClan(const Clan &);
    LocalPlayer &	playerOfWind(const Wind &);
    bool		client2Mahjong(const Avatar &, const ClientMessage &, ActionList &);
}

bool AI::mahjongTurn(const Wind & currentWind, const Avatar & avatar, const VecStones & trash,
    const WinRules & left, const WinRules & right, const WinRules & top, bool showGame, bool showKong, ActionList & actions)
{
    // simple AI
    actions.push_back(MahjongTurn(currentWind, Stone(), false, false));

    if(showGame)
    {
	GameData::client2Mahjong(avatar, ClientSayGame(), actions);
	GameData::client2Mahjong(avatar, ClientButtonGame(), actions);
	return true;
    }
    else
    if(showKong)
    {
	GameData::client2Mahjong(avatar, ClientSayKong(1), actions);
	GameData::client2Mahjong(avatar, ClientButtonKong2(), actions);
	return true;
    }

    // AI select
    WinRules other; other.reserve(12);
    other.insert(other.end(), left.begin(), left.end());
    other.insert(other.end(), right.begin(), right.end());
    other.insert(other.end(), top.begin(), top.end());

    LocalPlayer & player = GameData::playerOfAvatar(avatar);

    if(player.newStone.isValid())
    {
	player.stones.add(player.newStone);
	player.newStone = GameStone(Stone::None, true);
    }

    // cast or summon
    const AvatarInfo & avatarInfo = GameData::avatarInfo(avatar);

    if(! player.isCasted())
    {
	Creatures summons;
        Spells casts;

	// allow summon creatures
	for(auto & cr : avatarInfo.creatures)
	{
    	    const CreatureInfo & creatureInfo = GameData::creatureInfo(cr);

	    // allow summon
	    if(creatureInfo.cost <= player.points &&
		player.stones.allowCast(creatureInfo.stones)) summons.push_back(creatureInfo.id);
	}

	// allow cast spells
	for(auto & sp : avatarInfo.spells)
	{
    	    const SpellInfo & spellInfo = GameData::spellInfo(sp);

	    // allow cast
	    if(spellInfo.cost <= player.points &&
		player.stones.allowCast(spellInfo.stones)) casts.push_back(spellInfo.id);
	}

	if(summons.size() || casts.size())
	    mahjongSummonCast(avatar, summons, casts, actions);
    }

    // drop stone
    int dropIndex = mahjongSelect(player.stones, trash, other);
    return GameData::client2Mahjong(avatar, ClientDropIndex(dropIndex), actions);
}

struct ArmyFullHousePredicate
{
    const BattleArmy &army;

    ArmyFullHousePredicate(const BattleArmy & a) : army(a)
    {
    }

    bool operator() (const Land & land) const
    {
	const BattleParty* bp = army.findPartyConst(land);
	return bp && ! bp->canJoin();
    }
};

void AI::mahjongSummonCast(const Avatar & avatar, const Creatures & summons, const Spells & casts, ActionList & actions)
{
    const LocalPlayer & player = GameData::playerOfAvatar(avatar);

    FIXME("fixme add AI person priority action(cast or summon) to json...");
    bool summonPriority = summons.size();
    bool castPriority = false;

    if(summonPriority)
    {
	Lands powerLands = Lands::thisClan(player.clan).powerOnly();
	if(powerLands.size())
	{
	    const ArmyFullHousePredicate armyFullHouse(player.army);

	    // remove powerLands: if army full house
	    powerLands.resize(std::distance(powerLands.begin(),
			std::remove_if(powerLands.begin(), powerLands.end(), armyFullHouse)));

	    if(powerLands.size())
	    {
		auto land = Tools::random_n(powerLands.begin(), powerLands.end());
		auto creature = Tools::random_n(summons.begin(), summons.end());

		GameData::client2Mahjong(avatar, ClientSummonCreature(*creature, *land), actions);
		castPriority = false;
	    }
	    else
	    {
		DEBUG("summon false, powerLands is full house");
		castPriority = true;
	    }
	}
	else
	{
	    DEBUG("summon false, powerLands not found");
	    castPriority = true;
	}
    }

    if(castPriority)
    {
	DEBUG("cast spell priority");

	auto spell = Tools::random_n(casts.begin(), casts.end());

	if(spell != casts.end())
	{
	    const SpellInfo & spellInfo = GameData::spellInfo(*spell);

	    BattleTargets targets;

	    for(auto & other : GameData::gamers)
	    {
		for(auto & tgt : other.toBattleTargets())
		{
		    switch(spellInfo.target())
		    {
			case SpellTarget::Land: // Any | Party
			    targets << tgt;
			    break;

			case SpellTarget::Enemy:
			    if(player.clan != other.clan) targets << tgt;
			    break;

			case SpellTarget::Friendly:
			    if(player.clan == other.clan) targets << tgt;
			    break;

			case SpellTarget::Party:

			case SpellTarget::Any: // Friendly | Enemy
			case SpellTarget::MyPlayer:
			case SpellTarget::OtherPlayer:
			case SpellTarget::AllPlayers:
			default: break;
		    }
		}
	    }
/*
    BattleTargets targets;
    targets.reserve(10);

    if(spellInfo.target() == SpellTarget::Land)
    {
	for(auto it = GameData::gamers.begin(); it != GameData::gamers.end(); ++it)
	{
	    auto party = (*it).army.findPartyConst(land);
	    if(party) targets << party->toBattleTargets((*it).clan);
	}
    }
    else
    if(spellInfo.target() & SpellTarget::Party)
    {
	if(spellInfo.target() & SpellTarget::Friendly)
	{
	    auto party = client.army.findPartyConst(land);
	    if(party) targets << party->toBattleTargets(client.clan);
	}
	else
	if(spellInfo.target() & SpellTarget::Enemy)
	{
	    for(auto it = GameData::gamers.begin(); it != GameData::gamers.end(); ++it)
	        if(client.clan != (*it).clan)
	    {
		auto party = (*it).army.findPartyConst(land);
		if(party) targets << party->toBattleTargets((*it).clan);
	    }
	}
    }
    else
    if(unit)
    {
	BattleCreature* bcr = nullptr;

	for(auto it = GameData::gamers.begin(); it != GameData::gamers.end() && !bcr; ++it)
	{
	    auto party = (*it).army.findParty(land);
	    bcr = party ? party->findBattleUnit(unit) : nullptr;
	    if(bcr) targets.push_back(BattleTarget(bcr, (*it).clan, land));
	}
    }
*/

	    // Spell spell;
	    // Land land;
	    // BattleTarget uid;
	    // GameData::client2Mahjong(avatar, ClientCastSpell(spell, land, uid, actions);
	}
    }
}

void AI::mahjongOtherPass(const Wind & currentWind, ActionList & actions, const Wind & skip)
{
    for(auto & id : winds_all)
    {
	LocalPlayer & playerAI = GameData::playerOfWind(id);

	if(id == currentWind() || id == skip() || ! playerAI.isAI())
		continue;

	actions.push_back(MahjongPass(id));
    }
}

bool AI::mahjongGameKongPungChao(const Wind & currentWind, const Wind & roundWind, const Stone & dropStone, WinResults & winResult, ActionList & actions, bool sayOnly)
{
    // set game
    for(auto & id : winds_all)
    {
	if(id == currentWind())
		continue;

	LocalPlayer & playerAI = GameData::playerOfWind(id);

	if(playerAI.isAI() &&
	    playerAI.isWinMahjong(currentWind, roundWind, dropStone, & winResult))
	{
	    if(sayOnly)
		return GameData::client2Mahjong(playerAI.avatar, ClientSayGame(), actions);
	    else
		return GameData::client2Mahjong(playerAI.avatar, ClientButtonGame(), actions);
	}
    }

    // set kong, pung
    for(auto & id : winds_all)
    {
	if(id == currentWind())
		continue;

	LocalPlayer & playerAI = GameData::playerOfWind(id);

	if(playerAI.isAI())
	{
	    if(playerAI.isMahjongKong1(currentWind, dropStone))
	    {
		if(sayOnly)
		    return GameData::client2Mahjong(playerAI.avatar, ClientSayKong(2), actions);
		else
		    return GameData::client2Mahjong(playerAI.avatar, ClientButtonKong1(), actions);
	    }
	    else
	    if(playerAI.isMahjongPung(currentWind, dropStone))
	    {
		if(sayOnly)
		    return GameData::client2Mahjong(playerAI.avatar, ClientSayPung(), actions);
		else
		    return GameData::client2Mahjong(playerAI.avatar, ClientButtonPung(), actions);
	    }
	}
    }

    // set chao
    for(auto & id : winds_all)
    {
	if(id == currentWind())
	    continue;

	LocalPlayer & playerAI = GameData::playerOfWind(id);

	if(playerAI.isAI() &&
	    playerAI.isMahjongChao(currentWind, dropStone))
	{
	    if(sayOnly)
		return GameData::client2Mahjong(playerAI.avatar, ClientSayChao(), actions);
	    else
		return GameData::client2Mahjong(playerAI.avatar, ClientChaoVariant(255), actions);
	}
    }

    return false;
}

struct StoneCost
{
    GameStone		stone;
    int			index;
    int			cost;

    StoneCost() : index(-1), cost(0) {}
    StoneCost(const GameStone & gs, int pos, int val) : stone(gs), index(pos), cost(val) {}

    bool		operator< (const StoneCost & sc) const { return cost < sc.cost; }
};

int AI::mahjongSelect(const GameStones & stones, const VecStones & trash, const WinRules & other)
{
    std::multiset<StoneCost> result;

    for(auto it = stones.begin(); it != stones.end(); ++it)
    {
	const GameStone & stone = *it;
	int cost = 100;

	int count1 = std::count(stones.begin(), stones.end(), stone);
	int count2 = std::count(trash.begin(), trash.end(), stone) + std::count(other.begin(), other.end(), stone);

	// winds and dragons
	if(stone.isSpecial())
	{
	    // check: possible pung or kong
	    if(1 < count1)
	    {
		if(3 < count1 + count2)
		    cost -= 20 * count2;
		else
		    cost += 20 * count1;
	    }
	}
	else
	{
	    // check: possible pung or kong
	    if(1 < count1)
	    {
		if(3 < count1 + count2)
		    cost -= 10 * count2;
		else
		    cost += 10 * count1;
	    }

	    // check: possible chao
	    switch(stone.order())
	    {
    		case 1: // [1] 2 3
		cost += 10 * ((stones.findStone(stone.next()) ? 1 : 0) +
				    (stones.findStone(stone.next().next()) ? 1 : 0));
		break;

    		case 2: // 1 [2] 3, [2] 3 4
		cost += 10 * ((stones.findStone(stone.prev()) ? 1 : 0) + (stones.findStone(stone.next()) ? 1 : 0) +
				    (stones.findStone(stone.next().next()) ? 1 : 0));
		break;

    	    	case 3: // 1 2 [3], 2 [3] 4, [3] 4 5
    		case 4: // 2 3 [4], 3 [4] 5, [4] 5 6
    		case 5: // 3 4 [5], 4 [5] 6, [5] 6 7
    		case 6: // 4 5 [6], 5 [6] 7, [6] 7 8
    		case 7: // 5 6 [7], 6 [7] 8, [7] 8 9
		cost += 10 * ((stones.findStone(stone.prev().prev()) ? 1 : 0) + (stones.findStone(stone.prev()) ? 1 : 0) +
				    (stones.findStone(stone.next()) ? 1 : 0) + (stones.findStone(stone.next().next()) ? 1 : 0));
		break;

    		case 8: // 6 7 [8], 7 [8] 9
		cost += 10 * ((stones.findStone(stone.prev().prev()) ? 1 : 0) + (stones.findStone(stone.prev()) ? 1 : 0) +
				    (stones.findStone(stone.next()) ? 1 : 0));
		break;

    		case 9: // 7 8 [9]
		cost += 10 * ((stones.findStone(stone.prev().prev()) ? 1 : 0) +
				    (stones.findStone(stone.prev()) ? 1 : 0));
		break;

		default: break;
	    }
	}

	result.emplace(stone, std::distance(stones.begin(), it), cost);
    }

    if(result.size())
    {
	auto itbeg = result.begin();
	auto itend = result.upper_bound(*itbeg);

	std::vector<StoneCost> rnd; rnd.reserve(8);
	std::copy(itbeg, itend, std::back_inserter(rnd));
	std::random_shuffle(rnd.begin(), rnd.end());

	// first find casted
	auto itres = std::find_if(rnd.begin(), rnd.end(), [](const StoneCost & sc) { return sc.stone.isCasted(); });
	if(itres != rnd.end()) return (*itres).index;

	// after return normal
	return rnd.front().index;
    }

    // impossible ;)
    return Tools::rand(0, stones.size());
}

namespace AI
{
    Lands findTargetsFor(const BaseStat &, const Clan &);
}

Lands AI::findTargetsFor(const BaseStat & bs, const Clan & clan)
{
    Lands res;

    for(auto & land : Lands::enemyAroundOnly(clan))
	if(land() != Land::TowerOf4Winds)
    {
	const LandInfo & landInfo = GameData::landInfo(land);
	BaseStat stat = landInfo.stat;

	const BattleParty* party = GameData::getBattleArmy(clan).findPartyConst(land);
	if(party)
	    stat += party->toBaseStatSummary();

	if(bs.loyalty > stat.attack - bs.defense + stat.ranger &&
	    stat.loyalty < bs.attack - stat.defense + bs.ranger)
	    res.push_back(land);
    }

    return res;
}

void AI::adventureMove(const RemotePlayer & player, ActionList & actions)
{
    // FIXME: army
    // toLocalData(const Avatar &)

/*
    Lands Lands::pathfind(const Land &, const Land &);
    Lands Lands::thisClan(const Clan &);
    Lands Lands::enemyAroundOnly(const Clan &);

    const BattleParty* party = player.army.findPartyConst(*it);
*/

    for(auto & party : player.army)
    {
	if(party.moveTarget().isValid())
	{
	    Lands path = Lands::pathfind(party.land(), party.moveTarget());

	    if(path.empty())
	    {
		ERROR("path is empty" << ", " << "from: " << party.land().toString() << ", " << "to: " << party.moveTarget().toString());
		continue;
	    }

	    if(1 < party.movePoint() && 1 < path.size())
	    {
		//const CreatureInfo & info = GameData::creatureInfo(*it);
		bool info_fly = true;

		if(info_fly)
		{
		    // move to path[1]
		}
		else
		{
		    // if clan != path[0] move to path[0] else path[1]
		}
	    }
	    else
	    if(0 < party.movePoint())
	    {
		// move to path.front()
		// client2Adventure(avatar, ClientUnitMoved((*it).first, (*it).second), actions);
	    }
	}
	else
	// set target land
	{
	    Lands lands = AI::findTargetsFor(party.toBaseStatSummary(), player.clan);
	    std::random_shuffle(lands.begin(), lands.end());

	    if(lands.size())
	    {
                auto & tgtLand = lands.front();

		// FIXME: sort to distance
		DEBUG("avatar: " << player.avatar.toString() << ", " << "target: " << tgtLand.toString());
	    }
	}
    }
}
