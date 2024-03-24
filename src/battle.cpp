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

#include <random>
#include <algorithm>

#include "battle.h"

namespace Battle
{
    int			calculateDamage(const BattleUnit & skill1, const BattleUnit & skill2, int bonus);
    BattleStrike	applyRangerAttack(const BattleUnit & skill, BattleCreature & target);
    BattleStrikes	rangersAttack(const BattleCreatures & rangers, BattleParty & enemy);
    BattleStrikes	applyMeleeAttack(BattleUnit & skill1, BattleUnit & skill2, int bonus);
    BattleStrikes	meleeAttack(BattleUnit & skill, BattleParty & enemy);
    BattleStrikes	meleesAttack(BattleCreatures attackers, BattleParty & enemy);
    BattleStrikes	doTargetStrike(BattleCreature & bcr, const BattleCreatures & party, BattleCreature & target);
}

BattleStrike Battle::applyRangerAttack(const BattleUnit & skill, BattleCreature & target)
{
    int damage = skill.ranger();

    if(target.isAffectedSpell(Spell::ForceShield))
    {
	VERBOSE("Affected spell: Force Shield!");
	damage -= 1;
    }

    target.applyDamage(damage);

    DEBUG("attacker: " << skill.name() << ", " << "do damage: " << damage << ", " <<
	"target: " << target.name() << ", " << "loyalty: " << target.loyalty() << ", " <<
	"status: " << (target.isAlive() ? "is alive" : (target.isTown() ? " is captured" : " is dead")));

    return BattleStrike(skill, skill.ranger(), target, BattleStrike::Ranger);
}

BattleStrikes Battle::rangersAttack(const BattleCreatures & rangers, BattleParty & enemy)
{
    BattleStrikes res;

    std::random_device rd;
    std::mt19937 mtg(rd());

    for(auto & bcr : rangers)
    {
	BattleCreatures bcrs = enemy.toBattleCreatures(Specials() << Speciality::IgnoreMissiles, false);

        std::shuffle(bcrs.begin(), bcrs.end(), mtg);

	auto tgt = bcrs.size() ? bcrs.front() : nullptr;
	if(bcr && tgt) res << applyRangerAttack(*bcr, *tgt);
    }

    return res;
}

int Battle::calculateDamage(const BattleUnit & skill1, const BattleUnit & skill2, int bonus)
{
    int mighty_blow = 0;

    if(skill1.haveSpeciality(Speciality::MightyBlow))
    {
	SpecialityMightyBlow blow;

	if(blow.chance() > Tools::rand(1, 100))
	{
	    VERBOSE("Speciality: " << "Mighty Blow!");
	    mighty_blow = blow.strength();
	}
    }

    int damage = skill1.attack() + mighty_blow - skill2.defense();
    if(0 < bonus) damage += bonus;

    if(damage <= 0)
    {
	int rnd = Tools::rand(1, 100);

	switch(std::abs(damage))
        {
	    // 50%
	    case 0:	damage = 50 >= rnd ? 1 : 0; break;
	    // 25%
            case 1:	damage = 25 >= rnd ? 1 : 0; break;
	    // 12%
            case 2:	damage = 12 >= rnd ? 1 : 0; break;
	    // 6%
	    case 3:	damage = 6  >= rnd ? 1 : 0; break;
	    // 3%
	    case 4:	damage = 3  >= rnd ? 1 : 0; break;
	    // 1%
	    default:	damage = 55 == rnd ? 1 : 0; break;
	}
    }

    return damage;
}

BattleStrikes Battle::applyMeleeAttack(BattleUnit & skill1, BattleUnit & skill2, int bonus)
{
    BattleStrikes res;

    int damage = calculateDamage(skill1, skill2, bonus);
    skill2.applyDamage(damage);

    DEBUG("attacker: " << skill1.name() << ", " << "do damage: " << damage << ", " << "bonus: " << bonus << ", " <<
	"target: " << skill2.name() << ", " << "loyalty: " << skill2.loyalty() << ", " <<
	"status: " << (skill2.isAlive() ? "is alive" : (skill2.isTown() ? "is captured" : "is dead")));


    res << BattleStrike(skill1, damage, skill2, BattleStrike::Melee);

    if(! skill1.isTown() && skill2.haveSpeciality(Speciality::FireShield))
    {
	VERBOSE("Speciality: " << "Fire Shield!");

	damage = 1;
	skill1.applyDamage(damage);

	DEBUG("attacker: " << skill2.name() << ", " << "do damage: " << damage << ", " <<
	    "target: " << skill1.name() << ", " << "loyalty: " << skill1.loyalty() << ", " <<
	    "status: " << (skill1.isAlive() ? "is alive" : "is dead"));

	res << BattleStrike(skill2, damage, skill1, BattleStrike::FireShield);
    }

    return res;
}

BattleStrikes Battle::meleeAttack(BattleUnit & skill, BattleParty & enemy)
{
    BattleStrikes res;
    BattleCreatures bcrs = enemy.toBattleCreatures();

    std::random_device rd;
    std::mt19937 mtg(rd());
    std::shuffle(bcrs.begin(), bcrs.end(), mtg);

    auto target = bcrs.size() ? bcrs.front() : nullptr;

    if(target && target->isAlive() && skill.isAlive())
    {
	res << applyMeleeAttack(skill, *target, 0);

	if(target->isAlive())
	    res << applyMeleeAttack(*target, skill, 0);
    }

    return res;
}

BattleStrikes Battle::doTargetStrike(BattleCreature & bcr, const BattleCreatures & party, BattleCreature & target)
{
    BattleStrikes res;

    auto it = std::find(party.begin(), party.end(), & bcr);

    if(it != party.end())
    {
	// bonus: see comment below
	int bonus = party.size() - std::distance(party.begin(), it) - 1;
	res << applyMeleeAttack(bcr, target, bonus);
    }

    return res;
}

BattleStrikes Battle::meleesAttack(BattleCreatures attackers, BattleParty & enemy)
{
    BattleStrikes res;
    std::random_device rd;
    std::mt19937 mtg(rd());

    for(auto & bcr : attackers)
    {
	BattleCreatures bcrs = enemy.toBattleCreatures();
        std::shuffle(bcrs.begin(), bcrs.end(), mtg);

	auto tgt = bcrs.size() ? bcrs.front() : nullptr;
	if(bcr && tgt)
	{
	    if(tgt->haveSpeciality(Speciality::FirstStrike))
	    {
		VERBOSE("Speciality: " << "First Strike!");
		res << doTargetStrike(*tgt, bcrs, *bcr);

		if(bcr->isAlive())
		    res << doTargetStrike(*bcr, attackers, *tgt);
	    }
	    else
	    {
		res << doTargetStrike(*bcr, attackers, *tgt);

		if(tgt->isAlive())
		    res << doTargetStrike(*tgt, bcrs, *bcr);
	    }
	}
    }

    return res;
}

BattleStrikes Battle::doAttackParty(BattleParty & attackers, BattleTown & town, BattleParty* defenders)
{
    BattleStrikes res;

    /*
	The first round of combat is ranged combat.
	Ranged combat is simultaneous, and includes the territory ranged attack.
	The territory does ranged damage to one of the enemy creatures, and all creatures (attacking and defending)
	with a ranged attack rating of one or better shoot missiles at the enemy.
	Damage is then resolved for this round.
	Territories and creatures that ignore missiles are not targeted by ranged attacks.
	Apart from this, targets are chosen randomly by each creature.
	Target creature loyalty is reduced by the ranged attack strength of the opponent, with no defensive adjustment.
    */

    if(town.isRanger())
    {
	BattleCreatures bcrs = attackers.toBattleCreatures(Specials() << Speciality::IgnoreMissiles, false);

        std::random_device rd;
        std::mt19937 mtg(rd());
        std::shuffle(bcrs.begin(), bcrs.end(), mtg);

	auto target = bcrs.size() ? bcrs.front() : nullptr;

	if(target)
	{
	    res << applyRangerAttack(town, *target);
	    attackers.removeUnloyalty();
	}
    }

    if(defenders)
    {
	BattleCreatures bcrs1 = attackers.toBattleCreatures(Specials() << Speciality::RangerAttack, true);
	res << rangersAttack(bcrs1, *defenders);
	defenders->removeUnloyalty();

	BattleCreatures bcrs2 = defenders->toBattleCreatures(Specials() << Speciality::RangerAttack, true);
	res << rangersAttack(bcrs2, attackers);
	attackers.removeUnloyalty();
    }

    /*
	After the ranged combat round, melee combat commences.
	In melee combat, the team that holds the territory goes first.
	The first battle pits the leading creatures of the two parties.
	Melee attack value of a creature is raised by one for each additional creature in the party
	behind the one currently attacking.
	(Thus, a party of maximum size, 3 creatures, confers a bonus of 2 to the melee value of the leading creature.)
	There is no such bonus to the defense or loyalty values, except for parties containing 2 or more creatures
	with the merge special ability.
        In combat, if the melee strength of a combatant is less than or equal to the defense of the creature
	it is in combat with, the following table applies:

        Defense - Melee                 % chance to hit
                0                               50
                1                               25
                2                               12
                3                                6
                n                             1/(n+1)
*/
    while(attackers.count() && town.isAlive())
    {
	if(defenders && defenders->count())
	{
	    res << meleesAttack(defenders->toBattleCreatures(), attackers);
	    attackers.removeUnloyalty();
	    defenders->removeUnloyalty();
	}
	else
	{
	    res << meleeAttack(town, attackers);
	    attackers.removeUnloyalty();
	}
    }

    return res;
}
