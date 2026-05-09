#!/usr/bin/env python3
"""
Guild Tycoon: Age of Reckoning
A sophisticated Game Engine/GM for managing the world of Aethoria.
"""

import json
import base64
import random
from datetime import datetime
from typing import Dict, List, Optional, Tuple, Any
from dataclasses import dataclass, field, asdict
from enum import Enum

# ==================== ENUMS & CONSTANTS ====================

class ClassType(Enum):
    WARRIOR = "Warrior"
    MAGE = "Mage"
    HEALER = "Healer"

class SubClass(Enum):
    # Warrior
    DEFENDER = "Defender"
    BERSERKER = "Berserker"
    KNIGHT = "Knight"
    # Mage
    DESTRUCTION = "Destruction"
    ARCANE = "Arcane"
    SUMMONER = "Summoner"
    # Healer
    HOLY = "Holy"
    NATURE = "Nature"
    DARK = "Dark"

class Race(Enum):
    HUMAN = "Human"
    ELF = "Elf"
    DWARF = "Dwarf"
    UNDEAD = "Undead"
    DRAGONKIN = "Dragonkin"
    ORC = "Orc"

class MissionType(Enum):
    DUNGEON_CRAWL = "Dungeon Crawl"
    MONSTER_HUNT = "Monster Hunt"
    ESCORT = "Escort"
    SABOTAGE = "Sabotage"
    RESCUE = "Rescue"
    SCOUTING = "Scouting"
    DEMON_SUBJUGATION = "Demon Subjugation"

class ReputationRank(Enum):
    F = "F"
    E = "E"
    D = "D"
    C = "C"
    B = "B"
    A = "A"
    S = "S"

class BondTier(Enum):
    STRANGER = "Stranger"
    ACQUAINTANCE = "Acquaintance"
    FRIEND = "Friend"
    CLOSE_FRIEND = "Close Friend"
    BONDED = "Bonded"
    SOULBOUND = "Soulbound"

class MemberStatus(Enum):
    HEALTHY = "Healthy"
    INJURED = "Injured"
    TRAUMATIZED = "Traumatized"
    SCARRED = "Permanently Scarred"
    DEAD = "Dead"

# Race conflicts
RACE_CONFLICTS = {
    Race.UNDEAD: [Race.ELF],
    Race.ELF: [Race.UNDEAD],
    Race.DWARF: [Race.DRAGONKIN],
    Race.DRAGONKIN: [Race.DWARF],
}

# Bosses by region
BOSSES = {
    1: {"name": "Ignar", "element": "Flame", "level_range": (1, 10)},
    2: {"name": "Malgara", "element": "Tide", "level_range": (11, 20)},
    3: {"name": "Vorath", "element": "Undying", "level_range": (21, 35)},
    4: {"name": "Zekrath & Shadow Twins", "element": "Storm", "level_range": (36, 49)},
    5: {"name": "Drexarion (Gatekeeper) → Malvoros", "element": "Darkness", "level_range": (50, 100)},
}

REGIONS = {
    1: {"name": "Ashwood Forest", "level_range": (1, 10), "difficulty": 1.0},
    2: {"name": "Cursed Swamp", "level_range": (11, 20), "difficulty": 1.5},
    3: {"name": "Ruined Kingdom", "level_range": (21, 35), "difficulty": 2.0},
    4: {"name": "Demon Frontier", "level_range": (36, 49), "difficulty": 3.0},
    5: {"name": "Demon King Fortress", "level_range": (50, 100), "difficulty": 5.0},
}

TRAITS_POOL = [
    "Brave", "Fast Learner", "Coward", "Greedy", "Loyal", "Reckless",
    "Tactical", "Magical Affinity", "Strong Will", "Compassionate",
    "Cynical", "Optimistic", "Disciplined", "Wild", "Craftsman",
    "Leader", "Lone Wolf", "Team Player", "Ambitious", "Humble"
]

NPCS = [
    "Merchant Baldric",
    "Priestess Seraphina",
    "Black Market Vex",
    "Rival: Iron Fang Guild"
]

# ==================== DATA CLASSES ====================

@dataclass
class Trait:
    name: str
    effect: str

@dataclass
class Member:
    id: int
    name: str
    race: Race
    class_type: ClassType
    subclass: SubClass
    level: int = 1
    xp: int = 0
    morale: int = 75
    status: MemberStatus = MemberStatus.HEALTHY
    traits: List[str] = field(default_factory=list)
    bonds: Dict[int, int] = field(default_factory=dict)  # member_id -> bond_level
    gear_tier: int = 1
    stats: Dict[str, int] = field(default_factory=lambda: {"STR": 10, "DEX": 10, "INT": 10, "VIT": 10})
    
    def to_dict(self) -> dict:
        return {
            "id": self.id,
            "name": self.name,
            "race": self.race.value,
            "class_type": self.class_type.value,
            "subclass": self.subclass.value,
            "level": self.level,
            "xp": self.xp,
            "morale": self.morale,
            "status": self.status.value,
            "traits": self.traits,
            "bonds": self.bonds,
            "gear_tier": self.gear_tier,
            "stats": self.stats
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'Member':
        return cls(
            id=data["id"],
            name=data["name"],
            race=Race(data["race"]),
            class_type=ClassType(data["class_type"]),
            subclass=SubClass(data["subclass"]),
            level=data["level"],
            xp=data["xp"],
            morale=data["morale"],
            status=MemberStatus(data["status"]),
            traits=data["traits"],
            bonds={int(k): v for k, v in data["bonds"].items()},
            gear_tier=data["gear_tier"],
            stats=data["stats"]
        )

@dataclass
class GameState:
    gold: int = 1000
    day: int = 1
    reputation_rank: ReputationRank = ReputationRank.F
    fame_points: int = 0
    debt_counter: int = 0
    act: int = 1
    
    # Resources
    wood: int = 0
    stone: int = 0
    mana_crystal: int = 0
    herb: int = 0
    metal_ore: int = 0
    demon_shard: int = 0
    
    # Facilities
    guild_hall_tier: int = 1
    has_tavern: bool = False
    has_infirmary: bool = False
    has_alchemy_lab: bool = False
    has_diplomacy_room: bool = False
    has_training_ground: bool = False
    
    # Members
    members: List[Member] = field(default_factory=list)
    next_member_id: int = 3  # Start after initial 2 members
    
    # World progress
    defeated_bosses: List[int] = field(default_factory=list)
    current_region: int = 1
    
    # Settings
    auto_save: bool = True
    difficulty: str = "Normal"  # Easy, Normal, Hard
    sound_enabled: bool = True
    
    def to_dict(self) -> dict:
        return {
            "gold": self.gold,
            "day": self.day,
            "reputation_rank": self.reputation_rank.value,
            "fame_points": self.fame_points,
            "debt_counter": self.debt_counter,
            "act": self.act,
            "wood": self.wood,
            "stone": self.stone,
            "mana_crystal": self.mana_crystal,
            "herb": self.herb,
            "metal_ore": self.metal_ore,
            "demon_shard": self.demon_shard,
            "guild_hall_tier": self.guild_hall_tier,
            "has_tavern": self.has_tavern,
            "has_infirmary": self.has_infirmary,
            "has_alchemy_lab": self.has_alchemy_lab,
            "has_diplomacy_room": self.has_diplomacy_room,
            "has_training_ground": self.has_training_ground,
            "members": [m.to_dict() for m in self.members],
            "next_member_id": self.next_member_id,
            "defeated_bosses": self.defeated_bosses,
            "current_region": self.current_region,
            "auto_save": self.auto_save,
            "difficulty": self.difficulty,
            "sound_enabled": self.sound_enabled
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'GameState':
        state = cls()
        state.gold = data["gold"]
        state.day = data["day"]
        state.reputation_rank = ReputationRank(data["reputation_rank"])
        state.fame_points = data["fame_points"]
        state.debt_counter = data["debt_counter"]
        state.act = data["act"]
        state.wood = data["wood"]
        state.stone = data["stone"]
        state.mana_crystal = data["mana_crystal"]
        state.herb = data["herb"]
        state.metal_ore = data["metal_ore"]
        state.demon_shard = data["demon_shard"]
        state.guild_hall_tier = data["guild_hall_tier"]
        state.has_tavern = data["has_tavern"]
        state.has_infirmary = data["has_infirmary"]
        state.has_alchemy_lab = data["has_alchemy_lab"]
        state.has_diplomacy_room = data["has_diplomacy_room"]
        state.has_training_ground = data["has_training_ground"]
        state.members = [Member.from_dict(m) for m in data["members"]]
        state.next_member_id = data["next_member_id"]
        state.defeated_bosses = data["defeated_bosses"]
        state.current_region = data["current_region"]
        state.auto_save = data.get("auto_save", True)
        state.difficulty = data.get("difficulty", "Normal")
        state.sound_enabled = data.get("sound_enabled", True)
        return state

# ==================== GAME ENGINE ====================

class GuildTycoonEngine:
    def __init__(self):
        self.state = GameState()
        self.initialize_game()
    
    def initialize_game(self):
        """Initialize with starting conditions"""
        # Create initial members
        warrior = Member(
            id=1,
            name="Theron",
            race=Race.HUMAN,
            class_type=ClassType.WARRIOR,
            subclass=SubClass.DEFENDER,
            traits=random.sample(TRAITS_POOL, 3)
        )
        
        healer = Member(
            id=2,
            name="Lyra",
            race=Race.ELF,
            class_type=ClassType.HEALER,
            subclass=SubClass.HOLY,
            traits=random.sample(TRAITS_POOL, 3)
        )
        
        self.state.members = [warrior, healer]
        self.state.next_member_id = 3
        
        print("\n" + "="*60)
        print("🏰 GUILD TYCOON: AGE OF RECKONING")
        print("="*60)
        print("Welcome to Aethoria, Guild Master!")
        print(f"Your guild hall is ready with {self.state.gold} Gold.")
        print(f"Initial members: {warrior.name} ({warrior.subclass.value}) and {healer.name} ({healer.subclass.value})")
        print("="*60 + "\n")
    
    def save_game(self, filename: str = "savegame.json") -> str:
        """Save game state to base64 encoded string"""
        try:
            data = self.state.to_dict()
            json_str = json.dumps(data, indent=2)
            encoded = base64.b64encode(json_str.encode('utf-8')).decode('utf-8')
            
            # Also save to file
            with open(filename, 'w') as f:
                f.write(encoded)
            
            print(f"✅ Game saved successfully! (File: {filename})")
            print(f"📜 Save Code (copy this to load later):\n{encoded[:100]}...")
            return encoded
        except Exception as e:
            print(f"❌ Save failed: {e}")
            return ""
    
    def load_game(self, save_code: str = "", filename: str = "savegame.json") -> bool:
        """Load game state from base64 encoded string or file"""
        try:
            if not save_code:
                # Try to load from file
                with open(filename, 'r') as f:
                    save_code = f.read().strip()
            
            decoded = base64.b64decode(save_code).decode('utf-8')
            data = json.loads(decoded)
            self.state = GameState.from_dict(data)
            
            print(f"✅ Game loaded successfully!")
            print(f"Day: {self.state.day} | Gold: {self.state.gold} | Act: {self.state.act}")
            print(f"Members: {len(self.state.members)} | Rank: {self.state.reputation_rank.value}")
            return True
        except FileNotFoundError:
            print("❌ No save file found.")
            return False
        except Exception as e:
            print(f"❌ Load failed: {e}")
            return False
    
    def calculate_daily_upkeep(self) -> Tuple[int, str]:
        """Calculate daily expenses"""
        salary_per_member = 50
        maintenance = self.state.guild_hall_tier * 30
        facility_cost = sum([
            20 if self.state.has_tavern else 0,
            25 if self.state.has_infirmary else 0,
            30 if self.state.has_alchemy_lab else 0,
            25 if self.state.has_diplomacy_room else 0,
            20 if self.state.has_training_ground else 0
        ])
        
        total_upkeep = (len(self.state.members) * salary_per_member) + maintenance + facility_cost
        breakdown = f"Salaries: {len(self.state.members) * salary_per_member} | Maintenance: {maintenance} | Facilities: {facility_cost}"
        return total_upkeep, breakdown
    
    def process_day(self) -> Dict[str, Any]:
        """Process one game day"""
        result = {
            "day": self.state.day,
            "income": 0,
            "expenses": 0,
            "net_change": 0,
            "events": [],
            "member_updates": []
        }
        
        # Calculate income (based on reputation and facilities)
        base_income = 100 * (ord(self.state.reputation_rank.value) - ord('F') + 1)
        if self.state.has_tavern:
            base_income += 50
        result["income"] = base_income
        
        # Calculate expenses
        upkeep, breakdown = self.calculate_daily_upkeep()
        result["expenses"] = upkeep
        
        # Net change
        result["net_change"] = base_income - upkeep
        self.state.gold += result["net_change"]
        
        # Check debt
        if self.state.gold < 0:
            self.state.debt_counter += 1
            if self.state.debt_counter >= 3:
                result["events"].append("⚠️ WARNING: High debt! Members are striking!")
                # Reduce morale
                for member in self.state.members:
                    if member.status == MemberStatus.HEALTHY:
                        member.morale = max(0, member.morale - 15)
                        result["member_updates"].append(f"{member.name}: Morale -15")
            if self.state.debt_counter >= 7:
                result["events"].append("💀 GAME OVER: Your guild has gone bankrupt!")
                return result
        
        # Random events
        if random.random() < 0.3:  # 30% chance
            event = self.trigger_random_event()
            if event:
                result["events"].append(event)
        
        # Recover injured members
        if self.state.has_infirmary:
            for member in self.state.members:
                if member.status == MemberStatus.INJURED:
                    if random.random() < 0.5:
                        member.status = MemberStatus.HEALTHY
                        result["member_updates"].append(f"🏥 {member.name} recovered from injuries!")
        
        # Recover traumatized members
        if self.state.has_tavern:
            for member in self.state.members:
                if member.status == MemberStatus.TRAUMATIZED:
                    if random.random() < 0.4:
                        member.status = MemberStatus.HEALTHY
                        result["member_updates"].append(f"🍺 {member.name} recovered at the tavern!")
        
        self.state.day += 1
        return result
    
    def trigger_random_event(self) -> Optional[str]:
        """Trigger a random event"""
        events = [
            ("Mysterious Traveler", "A traveler offers rare herbs for sale. (+10 Herb, -100 Gold)", lambda: self.add_resource("herb", 10) or self.modify_gold(-100)),
            ("Fire!", "A fire breaks out in the guild hall! (-50 Gold, -1 Wood)", lambda: self.modify_gold(-50) or self.add_resource("wood", -1)),
            ("Rival Guild Taunt", "Iron Fang Guild mocks your progress. (-5 Fame)", lambda: self.modify_fame(-5)),
            ("Generous Donation", "A noble donates to your guild! (+200 Gold)", lambda: self.modify_gold(200)),
            ("Epidemic Warning", "Disease spreads in the region. Members lose morale.", lambda: self.reduce_all_morale(10)),
            ("Blood Moon", "Demons grow stronger! Expeditions become more dangerous but rewarding.", lambda: None),
            ("Festival", "The town celebrates! (+10 Morale all members, -30 Gold)", lambda: self.boost_all_morale(10) or self.modify_gold(-30)),
        ]
        
        event_name, description, effect = random.choice(events)
        try:
            effect()
            return f"📜 EVENT: {event_name} - {description}"
        except:
            return f"📜 EVENT: {event_name} - {description}"
    
    def modify_gold(self, amount: int):
        self.state.gold += amount
    
    def add_resource(self, resource: str, amount: int):
        setattr(self.state, resource, getattr(self.state, resource) + amount)
    
    def modify_fame(self, amount: int):
        self.state.fame_points += amount
        # Check for rank up
        self.check_reputation_rank()
    
    def check_reputation_rank(self):
        ranks = ['F', 'E', 'D', 'C', 'B', 'A', 'S']
        current_idx = ranks.index(self.state.reputation_rank.value)
        
        thresholds = [0, 100, 300, 600, 1000, 1500, 2500]
        
        for i, threshold in enumerate(thresholds):
            if self.state.fame_points >= threshold and i > current_idx:
                self.state.reputation_rank = ReputationRank(ranks[i])
                print(f"🎉 RANK UP! You are now Rank {ranks[i]}!")
                break
    
    def boost_all_morale(self, amount: int):
        for member in self.state.members:
            if member.status == MemberStatus.HEALTHY:
                member.morale = min(100, member.morale + amount)
    
    def reduce_all_morale(self, amount: int):
        for member in self.state.members:
            if member.status == MemberStatus.HEALTHY:
                member.morale = max(0, member.morale - amount)
    
    def calculate_tactical_score(self, party: List[Member], mission_difficulty: float) -> Tuple[float, str]:
        """Calculate tactical score for expedition"""
        if not party:
            return 0, "No party selected!"
        
        # Base stats - scaled better for early game
        total_stats = sum(
            sum(m.stats.values()) * max(1, m.level / 5) 
            for m in party if m.status == MemberStatus.HEALTHY
        )
        
        # Composition bonus
        classes = [m.class_type for m in party]
        comp_bonus = 1.0
        comp_desc = "Standard"
        
        warrior_count = classes.count(ClassType.WARRIOR)
        mage_count = classes.count(ClassType.MAGE)
        healer_count = classes.count(ClassType.HEALER)
        
        if warrior_count >= 1 and healer_count >= 1 and mage_count >= 1:
            comp_bonus = 1.15
            comp_desc = "Balanced Party (+15%)"
        elif mage_count >= 3:
            comp_bonus = 1.20
            comp_desc = "Glass Cannon (+20%, Risky)"
        elif warrior_count >= 3:
            comp_bonus = 1.10
            comp_desc = "Tank Wall (+10%)"
        elif healer_count >= 2:
            comp_bonus = 1.05
            comp_desc = "Sustain Focus (+5%)"
        elif warrior_count >= 1 and healer_count >= 1:
            comp_bonus = 1.08
            comp_desc = "Duo Synergy (+8%)"
        
        # Gear modifier
        gear_mod = 1.0 + (sum(m.gear_tier for m in party) / len(party)) * 0.1
        
        # Morale modifier
        avg_morale = sum(m.morale for m in party) / len(party)
        morale_mod = 0.5 + (avg_morale / 100)
        
        # RNG factor
        rng_factor = random.uniform(0.9, 1.1)
        
        ts = total_stats * comp_bonus * gear_mod * morale_mod * rng_factor
        ts /= mission_difficulty  # Adjust for difficulty
        
        breakdown = f"Stats: {total_stats:.1f} | Comp: {comp_desc} | Gear: x{gear_mod:.2f} | Morale: x{morale_mod:.2f} | RNG: x{rng_factor:.2f}"
        return ts, breakdown
    
    def run_expedition(self, party_ids: List[int], region: int, mission_type: MissionType) -> Dict[str, Any]:
        """Run an expedition mission"""
        result = {
            "success": False,
            "rewards": {},
            "casualties": [],
            "injuries": [],
            "message": ""
        }
        
        # Get party members
        party = [m for m in self.state.members if m.id in party_ids]
        if not party:
            result["message"] = "Invalid party selection!"
            return result
        
        # Check if members are available
        unavailable = [m.name for m in party if m.status != MemberStatus.HEALTHY]
        if unavailable:
            result["message"] = f"Unavailable members: {', '.join(unavailable)}"
            return result
        
        region_info = REGIONS.get(region, REGIONS[1])
        difficulty = region_info["difficulty"]
        
        # Calculate tactical score
        required_ts = difficulty * 50 * len(party)  # Reduced from 100 to 50 for better early game
        actual_ts, ts_breakdown = self.calculate_tactical_score(party, difficulty)
        
        result["ts_breakdown"] = ts_breakdown
        result["required_ts"] = required_ts
        result["actual_ts"] = actual_ts
        
        # Determine outcome
        success_chance = min(0.95, actual_ts / required_ts)
        
        voice_barks_success = [
            "Victory is ours!",
            "The demons tremble before us!",
            "Another triumph for the guild!",
            "Glory to Aethoria!"
        ]
        
        voice_barks_fail = [
            "We must retreat!",
            "This foe is too strong!",
            "Fall back! Fall back!",
            "The darkness prevails..."
        ]
        
        if random.random() < success_chance:
            result["success"] = True
            result["message"] = f"✅ {random.choice(voice_barks_success)}\n\nTactical Analysis:\n{ts_breakdown}"
            
            # Calculate rewards
            base_gold = int(50 * difficulty * (1 + len(party) * 0.2))
            base_xp = int(30 * difficulty)
            
            result["rewards"]["gold"] = base_gold
            result["rewards"]["xp"] = base_xp
            
            # Resource drops based on region
            if region == 1:
                result["rewards"]["wood"] = random.randint(2, 5)
            elif region == 2:
                result["rewards"]["herb"] = random.randint(2, 4)
            elif region == 3:
                result["rewards"]["stone"] = random.randint(2, 4)
                result["rewards"]["metal_ore"] = random.randint(1, 3)
            elif region == 4:
                result["rewards"]["mana_crystal"] = random.randint(1, 3)
                result["rewards"]["demon_shard"] = random.randint(0, 1)
            elif region == 5:
                result["rewards"]["demon_shard"] = random.randint(2, 5)
                result["rewards"]["mana_crystal"] = random.randint(3, 6)
            
            # Apply rewards
            self.modify_gold(result["rewards"]["gold"])
            self.modify_fame(int(10 * difficulty))
            
            for member in party:
                member.xp += result["rewards"]["xp"]
                # Check for level up
                while member.xp >= member.level * 100:
                    member.xp -= member.level * 100
                    member.level += 1
                    member.morale = min(100, member.morale + 10)
                    # Stat increase
                    for stat in member.stats:
                        member.stats[stat] += random.randint(1, 3)
            
            # Post-mission status check
            for member in party:
                roll = random.random()
                if roll < 0.1:  # 10% chance of injury
                    member.status = MemberStatus.INJURED
                    result["injuries"].append(member.name)
                elif roll < 0.15:  # 5% chance of trauma
                    member.status = MemberStatus.TRAUMATIZED
                    result["casualties"].append(f"{member.name} (Traumatized)")
                elif roll < 0.17:  # 2% chance of permanent scar
                    member.status = MemberStatus.SCARRED
                    new_trait = random.choice(["Haunted", "Battle-Scarred", "Cautious"])
                    member.traits.append(new_trait)
                    result["casualties"].append(f"{member.name} (Permanently Scarred: {new_trait})")
            
            # Check for boss fight
            if region in BOSSES and region not in self.state.defeated_bosses:
                if random.random() < 0.3:  # 30% chance to encounter boss
                    boss = BOSSES[region]
                    result["boss_encounter"] = boss
                    result["message"] += f"\n\n👹 BOSS ENCOUNTER: {boss['name']} ({boss['element']})!"
        
        else:
            result["success"] = False
            result["message"] = f"❌ {random.choice(voice_barks_fail)}\n\nTactical Analysis:\n{ts_breakdown}"
            
            # Failure consequences
            for member in party:
                member.morale = max(0, member.morale - 15)
                roll = random.random()
                if roll < 0.3:  # 30% chance of injury on failure
                    member.status = MemberStatus.INJURED
                    result["injuries"].append(member.name)
                elif roll < 0.4:  # 10% chance of death
                    member.status = MemberStatus.DEAD
                    result["casualties"].append(f"{member.name} (KIA)")
            
            self.modify_fame(-5)
        
        # Check for race conflicts
        for i, m1 in enumerate(party):
            for m2 in party[i+1:]:
                if m1.race in RACE_CONFLICTS and m2.race in RACE_CONFLICTS[m1.race]:
                    if not self.state.has_diplomacy_room:
                        m1.morale = max(0, m1.morale - 5)
                        m2.morale = max(0, m2.morale - 5)
                        result["message"] += f"\n⚠️ Race conflict between {m1.name} ({m1.race.value}) and {m2.name} ({m2.race.value})!"
        
        return result
    
    def recruit_member(self, name: str, race: Race, class_type: ClassType, subclass: SubClass, cost: int = 200) -> bool:
        """Recruit a new member"""
        if self.state.gold < cost:
            print(f"❌ Not enough gold! Need {cost} gold.")
            return False
        
        new_member = Member(
            id=self.state.next_member_id,
            name=name,
            race=race,
            class_type=class_type,
            subclass=subclass,
            traits=random.sample(TRAITS_POOL, 3)
        )
        
        self.state.members.append(new_member)
        self.state.next_member_id += 1
        self.modify_gold(-cost)
        
        print(f"✅ Recruited {name} ({race.value} {subclass.value}) for {cost} gold!")
        return True
    
    def build_facility(self, facility: str) -> bool:
        """Build a new facility"""
        costs = {
            "tavern": {"gold": 500, "wood": 100},
            "infirmary": {"gold": 600, "wood": 80, "herb": 50},
            "alchemy_lab": {"gold": 800, "wood": 100, "mana_crystal": 30},
            "diplomacy_room": {"gold": 700, "stone": 100},
            "training_ground": {"gold": 500, "wood": 150, "stone": 50}
        }
        
        if facility not in costs:
            print("❌ Invalid facility!")
            return False
        
        cost = costs[facility]
        
        # Check resources
        if self.state.gold < cost["gold"]:
            print(f"❌ Not enough gold! Need {cost['gold']}.")
            return False
        
        for resource, amount in cost.items():
            if resource == "gold":
                continue
            if getattr(self.state, resource, 0) < amount:
                print(f"❌ Not enough {resource}! Need {amount}.")
                return False
        
        # Deduct resources
        self.modify_gold(-cost["gold"])
        for resource, amount in cost.items():
            if resource != "gold":
                self.add_resource(resource, -amount)
        
        # Build facility
        setattr(self.state, f"has_{facility}", True)
        print(f"✅ Built {facility.capitalize()}!")
        return True
    
    def upgrade_guild_hall(self) -> bool:
        """Upgrade guild hall tier"""
        cost = self.state.guild_hall_tier * 1000
        if self.state.gold < cost:
            print(f"❌ Not enough gold! Need {cost}.")
            return False
        
        self.modify_gold(-cost)
        self.state.guild_hall_tier += 1
        print(f"✅ Guild Hall upgraded to Tier {self.state.guild_hall_tier}!")
        return True
    
    def view_member(self, member_id: int):
        """View detailed member info"""
        member = next((m for m in self.state.members if m.id == member_id), None)
        if not member:
            print("❌ Member not found!")
            return
        
        print(f"\n{'='*40}")
        print(f"👤 {member.name} (ID: {member.id})")
        print(f"{'='*40}")
        print(f"Race: {member.race.value} | Class: {member.class_type.value} ({member.subclass.value})")
        print(f"Level: {member.level} | XP: {member.xp}/{member.level * 100}")
        print(f"Status: {member.status.value} | Morale: {member.morale}/100")
        print(f"Gear Tier: T{member.gear_tier}")
        print(f"Traits: {', '.join(member.traits)}")
        print(f"Stats: STR {member.stats['STR']} | DEX {member.stats['DEX']} | INT {member.stats['INT']} | VIT {member.stats['VIT']}")
        if member.bonds:
            print("Bonds:")
            for bond_id, level in member.bonds.items():
                bond_member = next((m for m in self.state.members if m.id == bond_id), None)
                if bond_member:
                    print(f"  - {bond_member.name}: {BondTier(min(6, level)).value}")
        print(f"{'='*40}\n")
    
    def display_status(self):
        """Display current game status"""
        print("\n" + "="*60)
        print(f"📊 DAY {self.state.day} | ACT {self.state.act}")
        print("="*60)
        print(f"💰 Gold: {self.state.gold} | Rank: {self.state.reputation_rank.value} ({self.state.fame_points} Fame)")
        print(f"🏰 Guild Hall: Tier {self.state.guild_hall_tier}")
        print(f"👥 Members: {len(self.state.members)}")
        
        # Resources
        resources = [
            f"🪵 Wood: {self.state.wood}",
            f"🪨 Stone: {self.state.stone}",
            f"💎 Mana Crystal: {self.state.mana_crystal}",
            f"🌿 Herb: {self.state.herb}",
            f"⛏️ Metal Ore: {self.state.metal_ore}",
            f"🔮 Demon Shard: {self.state.demon_shard}"
        ]
        print(" | ".join(resources))
        
        # Facilities
        facilities = []
        if self.state.has_tavern: facilities.append("🍺 Tavern")
        if self.state.has_infirmary: facilities.append("🏥 Infirmary")
        if self.state.has_alchemy_lab: facilities.append("⚗️ Alchemy Lab")
        if self.state.has_diplomacy_room: facilities.append("🤝 Diplomacy Room")
        if self.state.has_training_ground: facilities.append("🎯 Training Ground")
        
        if facilities:
            print(f"🏢 Facilities: {', '.join(facilities)}")
        
        # Members summary
        print("\n👥 MEMBERS:")
        for member in self.state.members:
            status_icon = "✅" if member.status == MemberStatus.HEALTHY else "⚠️"
            print(f"  {status_icon} {member.name} (Lv.{member.level} {member.subclass.value}) - Morale: {member.morale} | Status: {member.status.value}")
        
        # Current region
        region = REGIONS.get(self.state.current_region, REGIONS[1])
        print(f"\n🗺️ Current Region: {region['name']} (Lv {region['level_range'][0]}-{region['level_range'][1]})")
        
        if self.state.defeated_bosses:
            defeated = [BOSSES[r]["name"] for r in self.state.defeated_bosses]
            print(f"⚔️ Defeated Bosses: {', '.join(defeated)}")
        
        print("="*60 + "\n")
    
    def show_main_menu(self):
        """Display main menu"""
        print("\n" + "="*60)
        print("🎮 MAIN MENU")
        print("="*60)
        print("1. 📅 Process Day (Income/Expenses)")
        print("2. ⚔️ Expedition")
        print("3. 👥 Manage Members")
        print("4. 🏗️ Build/Upgrade")
        print("5. 📦 Save Game")
        print("6. 📂 Load Game")
        print("7. ⚙️ Settings")
        print("8. ℹ️ Credits")
        print("9. 📊 View Status")
        print("0. 🚪 Quit")
        print("="*60)
    
    def show_expedition_menu(self):
        """Display expedition menu"""
        print("\n" + "="*60)
        print("⚔️ EXPEDITION MENU")
        print("="*60)
        
        print("\n🗺️ REGIONS:")
        for region_id, info in REGIONS.items():
            boss = BOSSES.get(region_id, {})
            boss_str = f" | Boss: {boss.get('name', 'None')}" if region_id not in self.state.defeated_bosses else " ✅ Boss Defeated"
            status = "🔒" if region_id > self.state.current_region else "🔓"
            print(f"  {status} {region_id}. {info['name']} (Lv {info['level_range'][0]}-{info['level_range'][1]}){boss_str}")
        
        print("\n📜 MISSION TYPES:")
        for i, mission in enumerate(MissionType, 1):
            print(f"  {i}. {mission.value}")
        
        print("\n👥 AVAILABLE MEMBERS:")
        available = [m for m in self.state.members if m.status == MemberStatus.HEALTHY]
        for member in available:
            print(f"  [{member.id}] {member.name} (Lv.{member.level} {member.subclass.value})")
        
        if not available:
            print("  ❌ No healthy members available!")
            return
        
        print("\n0. Back")
        print("="*60)
        
        # Get user input
        try:
            region = int(input("\nSelect region (0 to cancel): "))
            if region == 0:
                return
            if region < 1 or region > 5:
                print("❌ Invalid region!")
                return
            if region > self.state.current_region:
                print("❌ Region locked! Defeat previous boss first.")
                return
            
            mission_choice = int(input("Select mission type (1-7): "))
            if mission_choice < 1 or mission_choice > 7:
                print("❌ Invalid mission type!")
                return
            mission_type = list(MissionType)[mission_choice - 1]
            
            party_input = input("Select party member IDs (comma-separated, e.g., 1,2): ")
            party_ids = [int(x.strip()) for x in party_input.split(",") if x.strip()]
            
            if not party_ids or len(party_ids) > 4 or len(party_ids) < 1:
                print("❌ Invalid party size! Must be 1-4 members.")
                return
            
            # Run expedition
            result = self.run_expedition(party_ids, region, mission_type)
            
            print("\n" + "="*60)
            print("📜 EXPEDITION RESULT")
            print("="*60)
            print(result["message"])
            
            if result["success"]:
                print("\n🎁 REWARDS:")
                for resource, amount in result["rewards"].items():
                    print(f"  +{amount} {resource.capitalize()}")
            
            if result["injuries"]:
                print(f"\n🏥 INJURED: {', '.join(result['injuries'])}")
            
            if result["casualties"]:
                print(f"\n💀 CASUALTIES: {', '.join(result['casualties'])}")
            
            if "boss_encounter" in result:
                boss = result["boss_encounter"]
                print(f"\n👹 BOSS DEFEATED: {boss['name']}!")
                self.state.defeated_bosses.append(region)
                
                # Unlock next region
                if region < 5:
                    self.state.current_region = region + 1
                    print(f"🔓 New region unlocked: {REGIONS[region + 1]['name']}!")
                
                # Act progression
                if len(self.state.defeated_bosses) >= 3 and self.state.act == 1:
                    self.state.act = 2
                    print("\n🎭 ACT 2 BEGIN: The Demon King's raids begin!")
                elif len(self.state.defeated_bosses) >= 5 and self.state.act == 2:
                    self.state.act = 3
                    print("\n🎭 ACT 3 BEGIN: Prepare for the final siege!")
            
            print("="*60)
            
        except ValueError:
            print("❌ Invalid input!")
    
    def show_members_menu(self):
        """Display members management menu"""
        while True:
            print("\n" + "="*60)
            print("👥 MEMBER MANAGEMENT")
            print("="*60)
            print("1. View Member Details")
            print("2. Recruit New Member")
            print("3. Train Member (50 Gold)")
            print("4. Equip Gear")
            print("0. Back")
            print("="*60)
            
            choice = input("Select option: ")
            
            if choice == "1":
                member_id = int(input("Enter member ID: "))
                self.view_member(member_id)
            
            elif choice == "2":
                name = input("Enter name: ")
                print("\nRACES:")
                for i, race in enumerate(Race, 1):
                    print(f"  {i}. {race.value}")
                race_choice = int(input("Select race (1-6): "))
                race = list(Race)[race_choice - 1]
                
                print("\nCLASSES:")
                for i, cls in enumerate(ClassType, 1):
                    print(f"  {i}. {cls.value}")
                class_choice = int(input("Select class (1-3): "))
                class_type = list(ClassType)[class_choice - 1]
                
                subclasses = {
                    ClassType.WARRIOR: [SubClass.DEFENDER, SubClass.BERSERKER, SubClass.KNIGHT],
                    ClassType.MAGE: [SubClass.DESTRUCTION, SubClass.ARCANE, SubClass.SUMMONER],
                    ClassType.HEALER: [SubClass.HOLY, SubClass.NATURE, SubClass.DARK]
                }
                
                print("\nSUBCLASSES:")
                for i, sub in enumerate(subclasses[class_type], 1):
                    print(f"  {i}. {sub.value}")
                sub_choice = int(input("Select subclass: "))
                subclass = subclasses[class_type][sub_choice - 1]
                
                self.recruit_member(name, race, class_type, subclass)
            
            elif choice == "3":
                member_id = int(input("Enter member ID: "))
                member = next((m for m in self.state.members if m.id == member_id), None)
                if member and self.state.gold >= 50:
                    self.modify_gold(-50)
                    member.xp += 20
                    member.morale = min(100, member.morale + 5)
                    print(f"✅ {member.name} trained! (+20 XP, +5 Morale)")
                else:
                    print("❌ Invalid member or not enough gold!")
            
            elif choice == "4":
                print("Gear upgrading coming soon!")
            
            elif choice == "0":
                break
    
    def show_build_menu(self):
        """Display build/upgrade menu"""
        while True:
            print("\n" + "="*60)
            print("🏗️ BUILD & UPGRADE")
            print("="*60)
            print("1. Upgrade Guild Hall")
            print("2. Build Tavern (500g, 100w)")
            print("3. Build Infirmary (600g, 80w, 50h)")
            print("4. Build Alchemy Lab (800g, 100w, 30mc)")
            print("5. Build Diplomacy Room (700g, 100s)")
            print("6. Build Training Ground (500g, 150w, 50s)")
            print("0. Back")
            print("="*60)
            
            choice = input("Select option: ")
            
            if choice == "1":
                self.upgrade_guild_hall()
            elif choice == "2":
                self.build_facility("tavern")
            elif choice == "3":
                self.build_facility("infirmary")
            elif choice == "4":
                self.build_facility("alchemy_lab")
            elif choice == "5":
                self.build_facility("diplomacy_room")
            elif choice == "6":
                self.build_facility("training_ground")
            elif choice == "0":
                break
    
    def show_settings_menu(self):
        """Display settings menu"""
        while True:
            print("\n" + "="*60)
            print("⚙️ SETTINGS")
            print("="*60)
            print(f"1. Auto-Save: {'ON' if self.state.auto_save else 'OFF'}")
            print(f"2. Difficulty: {self.state.difficulty}")
            print(f"3. Sound: {'ON' if self.state.sound_enabled else 'OFF'}")
            print("0. Back")
            print("="*60)
            
            choice = input("Select option: ")
            
            if choice == "1":
                self.state.auto_save = not self.state.auto_save
                print(f"✅ Auto-Save toggled: {'ON' if self.state.auto_save else 'OFF'}")
            elif choice == "2":
                difficulties = ["Easy", "Normal", "Hard"]
                print("\nDifficulties:")
                for i, diff in enumerate(difficulties, 1):
                    print(f"  {i}. {diff}")
                diff_choice = int(input("Select difficulty: "))
                if 1 <= diff_choice <= 3:
                    self.state.difficulty = difficulties[diff_choice - 1]
                    print(f"✅ Difficulty set to {self.state.difficulty}")
            elif choice == "3":
                self.state.sound_enabled = not self.state.sound_enabled
                print(f"✅ Sound toggled: {'ON' if self.state.sound_enabled else 'OFF'}")
            elif choice == "0":
                break
    
    def show_credits(self):
        """Display credits"""
        print("\n" + "="*60)
        print("🎮 GUILD TYCOON: AGE OF RECKONING")
        print("="*60)
        print("\n📜 CREDITS")
        print("="*60)
        print("\n🎨 Created by:")
        print("   • Horazonka aka Habibi")
        print("   • Nuhazakka")
        print("   • Arifin")
        print("\n🎵 Special Thanks:")
        print("   • All testers and supporters")
        print("   • The Aethoria Community")
        print("\n© 2024 Guild Tycoon Studios")
        print("="*60 + "\n")
    
    def run(self):
        """Main game loop"""
        print("\n🎮 Welcome to Guild Tycoon: Age of Reckoning!")
        print("Type 'load' to load a saved game or press Enter to start new.\n")
        
        init_input = input("> ").strip().lower()
        if init_input == "load":
            save_code = input("Enter save code or press Enter to load from file: ").strip()
            if save_code or self.load_game():
                pass
            else:
                print("Starting new game...")
        elif init_input.startswith("load "):
            save_code = init_input[5:].strip()
            self.load_game(save_code)
        
        while True:
            self.display_status()
            self.show_main_menu()
            
            choice = input("\nSelect option: ").strip()
            
            if choice == "1":
                day_result = self.process_day()
                print("\n" + "="*60)
                print(f"📅 DAY {day_result['day']} PROCESSED")
                print("="*60)
                print(f"💰 Income: +{day_result['income']} Gold")
                print(f"💸 Expenses: -{day_result['expenses']} Gold ({self.calculate_daily_upkeep()[1]})")
                print(f"📊 Net Change: {day_result['net_change']:+d} Gold")
                
                if day_result["events"]:
                    print("\n📜 EVENTS:")
                    for event in day_result["events"]:
                        print(f"  {event}")
                
                if day_result["member_updates"]:
                    print("\n👥 UPDATES:")
                    for update in day_result["member_updates"]:
                        print(f"  {update}")
                
                print("="*60)
                
                if self.state.auto_save:
                    self.save_game()
            
            elif choice == "2":
                self.show_expedition_menu()
            
            elif choice == "3":
                self.show_members_menu()
            
            elif choice == "4":
                self.show_build_menu()
            
            elif choice == "5":
                self.save_game()
            
            elif choice == "6":
                save_code = input("Enter save code: ").strip()
                self.load_game(save_code)
            
            elif choice == "7":
                self.show_settings_menu()
            
            elif choice == "8":
                self.show_credits()
            
            elif choice == "9":
                self.display_status()
            
            elif choice == "0":
                print("\n👋 Thank you for playing Guild Tycoon: Age of Reckoning!")
                print("May your guild prosper in Aethoria!\n")
                break
            
            else:
                print("❌ Invalid option!")

# ==================== ENTRY POINT ====================

if __name__ == "__main__":
    game = GuildTycoonEngine()
    game.run()
