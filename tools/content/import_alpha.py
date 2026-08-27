#!/usr/bin/env python3
"""Generate compatibility catalogs from an original SMAC alpha.txt file."""

from __future__ import annotations

import argparse
import csv
import re
from dataclasses import dataclass
from pathlib import Path


TECHNOLOGY_ID_OVERRIDES = {
    "Secrets of the Human Brain": "SecretsHumanBrain",
}

TECHNOLOGY_COST_OVERRIDES = {
    "Biogenetics": 30,
    "IndustrialBase": 50,
    "InformationNetworks": 40,
    "AppliedPhysics": 50,
    "SocialPsych": 40,
    "DoctrineMobility": 30,
    "CentauriEcology": 20,
    "PlanetaryNetworks": 50,
    "DoctrineLoyalty": 60,
    "IndustrialEconomics": 70,
    "SecretsHumanBrain": 80,
}


@dataclass(frozen=True)
class TechnologyRow:
    name: str
    code: str
    prerequisite_codes: tuple[str, ...]
    flags: int


@dataclass(frozen=True)
class FacilityRow:
    name: str
    mineral_cost: int
    maintenance: int
    prerequisite_code: str | None
    obsolete_code: str | None
    effect: str
    kind: str


@dataclass(frozen=True)
class ChassisRow:
    name: str
    speed: int
    triad: int
    range: int
    missile: bool
    cargo: int
    cost: int
    prerequisite_code: str


@dataclass(frozen=True)
class ReactorRow:
    name: str
    power: int
    prerequisite_code: str


@dataclass(frozen=True)
class WeaponRow:
    name: str
    short_name: str
    offense: int
    mode: int
    cost: int
    icon: int
    prerequisite_code: str


@dataclass(frozen=True)
class ArmorRow:
    name: str
    short_name: str
    defense: int
    mode: int
    cost: int
    prerequisite_code: str


@dataclass(frozen=True)
class AbilityRow:
    name: str
    cost: int
    prerequisite_code: str
    abbreviation: str
    flags: str
    effect: str


@dataclass(frozen=True)
class UnitRow:
    name: str
    chassis_name: str
    weapon_name: str
    armor_name: str
    plan: int
    mineral_cost: int
    cargo: int
    prerequisite_code: str
    icon: int
    ability_flags: str
    hidden: bool


def canonical_id(name: str) -> str:
    if name in TECHNOLOGY_ID_OVERRIDES:
        return TECHNOLOGY_ID_OVERRIDES[name]
    expanded = name.replace("Adv.", "Advanced")
    words = re.findall(r"[A-Za-z0-9]+", expanded)
    return "".join(word[:1].upper() + word[1:] for word in words)


def read_section(path: Path, section: str) -> list[str]:
    lines = path.read_text(encoding="latin-1").splitlines()
    marker = f"#{section}"
    try:
        start = lines.index(marker) + 1
    except ValueError as exc:
        raise ValueError(f"{path} has no {marker} section") from exc
    result: list[str] = []
    for line in lines[start:]:
        if line.startswith("#"):
            break
        stripped = line.strip()
        if stripped and not stripped.startswith(";"):
            result.append(line)
    return result


def read_technologies(path: Path) -> list[TechnologyRow]:
    technologies: list[TechnologyRow] = []
    for line in read_section(path, "TECHNOLOGY"):
        row = next(csv.reader([line], skipinitialspace=True))
        if len(row) < 9:
            raise ValueError(f"invalid TECHNOLOGY row: {line}")
        name = row[0].strip()
        if name.lower() == "deleted" or name.startswith("User Technology "):
            continue
        prerequisite_codes = tuple(
            code for code in (row[6].strip(), row[7].strip()) if code != "None"
        )
        technologies.append(
            TechnologyRow(
                name=name,
                code=row[1].strip(),
                prerequisite_codes=prerequisite_codes,
                flags=int(row[8].strip(), 2),
            )
        )
    return technologies


def read_facilities(path: Path) -> list[FacilityRow]:
    facilities: list[FacilityRow] = []
    for line in read_section(path, "FACILITIES"):
        row = next(csv.reader([line], skipinitialspace=True))
        if len(row) < 6:
            raise ValueError(f"invalid FACILITIES row: {line}")
        prerequisite_code = row[3].strip()
        obsolete_code = row[4].strip()
        kind = "project" if len(row) >= 11 else "facility"
        effect_fields = row[5:-5] if kind == "project" else row[5:]
        facilities.append(
            FacilityRow(
                name=row[0].strip(),
                mineral_cost=int(row[1].strip()) * 10,
                maintenance=int(row[2].strip()),
                prerequisite_code=(
                    None if prerequisite_code == "None" else prerequisite_code
                ),
                obsolete_code=(
                    None if obsolete_code == "Disable" else obsolete_code
                ),
                effect=", ".join(field.strip() for field in effect_fields),
                kind=kind,
            )
        )
    return facilities


def read_csv_section(path: Path, section: str) -> list[list[str]]:
    rows: list[list[str]] = []
    for line in read_section(path, section):
        source = line.split(";", 1)[0].strip()
        if not source:
            continue
        rows.append(
            [value.strip() for value in next(csv.reader([source], skipinitialspace=True))]
        )
    return rows


def read_unit_catalog(
    path: Path,
) -> tuple[
    list[ChassisRow],
    list[ReactorRow],
    list[WeaponRow],
    list[ArmorRow],
    list[AbilityRow],
    list[UnitRow],
]:
    chassis = [
        ChassisRow(
            name=row[0],
            speed=int(row[8]),
            triad=int(row[9]),
            range=int(row[10]),
            missile=bool(int(row[11])),
            cargo=int(row[12]),
            cost=int(row[13]),
            prerequisite_code=row[14],
        )
        for row in read_csv_section(path, "CHASSIS")
    ]
    reactors = [
        ReactorRow(name=row[0], power=int(row[2]), prerequisite_code=row[3])
        for row in read_csv_section(path, "REACTORS")
    ]
    weapons = [
        WeaponRow(
            name=row[0],
            short_name=row[1],
            offense=int(row[2]),
            mode=int(row[3]),
            cost=int(row[4]),
            icon=int(row[5]),
            prerequisite_code=row[6],
        )
        for row in read_csv_section(path, "WEAPONS")
    ]
    armors = [
        ArmorRow(
            name=row[0],
            short_name=row[1],
            defense=int(row[2]),
            mode=int(row[3]),
            cost=int(row[4]),
            prerequisite_code=row[5],
        )
        for row in read_csv_section(path, "DEFENSES")
    ]
    abilities = [
        AbilityRow(
            name=row[0],
            cost=int(row[1]),
            prerequisite_code=row[2],
            abbreviation=row[3],
            flags=row[4],
            effect=row[5],
        )
        for row in read_csv_section(path, "ABILITIES")
    ]
    unit_rows = read_csv_section(path, "UNITS")
    declared_unit_count = int(unit_rows.pop(0)[0])
    units: list[UnitRow] = []
    for row in unit_rows:
        hidden = row[0].startswith("*")
        units.append(
            UnitRow(
                name=row[0].lstrip("*"),
                chassis_name=row[1],
                weapon_name=row[2],
                armor_name=row[3],
                plan=int(row[4]),
                mineral_cost=int(row[5]) * 10,
                cargo=int(row[6]),
                prerequisite_code=row[7],
                icon=int(row[8]),
                ability_flags=row[9],
                hidden=hidden,
            )
        )
    if len(units) != declared_unit_count:
        raise ValueError(
            f"UNITS declares {declared_unit_count} rows but contains {len(units)}"
        )
    return chassis, reactors, weapons, armors, abilities, units


def quote(value: str) -> str:
    return "'" + value.replace("\\", "\\\\").replace("'", "\\'") + "'"


def generate_technology_catalog(rows: list[TechnologyRow]) -> str:
    code_to_id: dict[str, str] = {}
    for row in rows:
        technology_id = canonical_id(row.name)
        if row.code in code_to_id:
            raise ValueError(f"duplicate technology code: {row.code}")
        if technology_id in code_to_id.values():
            raise ValueError(f"duplicate technology id: {technology_id}")
        code_to_id[row.code] = technology_id

    prerequisites: dict[str, tuple[str, ...]] = {}
    names: dict[str, str] = {}
    commerce_bonuses: dict[str, int] = {}
    order: list[str] = []
    for row in rows:
        technology_id = code_to_id[row.code]
        try:
            prerequisite_ids = tuple(
                code_to_id[code] for code in row.prerequisite_codes
            )
        except KeyError as exc:
            raise ValueError(
                f"{technology_id} references missing technology code {exc.args[0]}"
            ) from exc
        prerequisites[technology_id] = prerequisite_ids
        names[technology_id] = row.name
        commerce_bonuses[technology_id] = 1 if row.flags & (1 << 2) else 0
        order.append(technology_id)

    tiers: dict[str, int] = {}
    resolving: set[str] = set()

    def get_tier(technology_id: str) -> int:
        if technology_id in tiers:
            return tiers[technology_id]
        if technology_id in resolving:
            raise ValueError(f"technology dependency cycle at {technology_id}")
        resolving.add(technology_id)
        prerequisite_ids = prerequisites[technology_id]
        tier = 0 if not prerequisite_ids else 1 + max(
            get_tier(prerequisite_id) for prerequisite_id in prerequisite_ids
        )
        resolving.remove(technology_id)
        tiers[technology_id] = tier
        return tier

    for technology_id in order:
        get_tier(technology_id)

    output = [
        "// Generated by tools/content/import_alpha.py from the original SMAC alpha.txt.",
        "// This catalog contains gameplay compatibility metadata only.",
        "const definitions = {",
    ]
    for technology_id in order:
        cost = TECHNOLOGY_COST_OVERRIDES.get(
            technology_id,
            20 + tiers[technology_id] * 10,
        )
        prerequisite_list = ", ".join(
            quote(prerequisite_id)
            for prerequisite_id in prerequisites[technology_id]
        )
        output.extend(
            [
                f"\t{technology_id}: {{",
                f"\t\tid: {quote(technology_id)},",
                f"\t\tname: {quote(names[technology_id])},",
                f"\t\tcost: {cost},",
                f"\t\tcommerce_bonus: {commerce_bonuses[technology_id]},",
                f"\t\tprerequisites: [{prerequisite_list}],",
                "\t},",
            ]
        )
    output.extend(
        [
            "};",
            "",
            "const order = [",
            *(f"\t{quote(technology_id)}," for technology_id in order),
            "];",
            "",
            "return {definitions: definitions, order: order};",
            "",
        ]
    )
    return "\n".join(output)


def generate_facility_catalog(
    facilities: list[FacilityRow],
    technologies: list[TechnologyRow],
) -> str:
    code_to_id = {
        technology.code: canonical_id(technology.name)
        for technology in technologies
    }
    entries: list[dict[str, str | int]] = []
    seen_ids: set[str] = set()
    for facility in facilities:
        facility_id = canonical_id(facility.name)
        if facility_id in seen_ids:
            raise ValueError(f"duplicate facility id: {facility_id}")
        seen_ids.add(facility_id)
        try:
            required_technology = (
                ""
                if facility.prerequisite_code is None
                else code_to_id[facility.prerequisite_code]
            )
            obsolete_technology = (
                ""
                if facility.obsolete_code is None
                else code_to_id[facility.obsolete_code]
            )
        except KeyError as exc:
            raise ValueError(
                f"{facility_id} references missing technology code {exc.args[0]}"
            ) from exc
        entries.append(
            {
                "id": facility_id,
                "name": facility.name,
                "kind": facility.kind,
                "mineral_cost": facility.mineral_cost,
                "energy_maintenance": facility.maintenance,
                "required_technology": required_technology,
                "obsolete_technology": obsolete_technology,
                "effect": facility.effect,
            }
        )

    output = [
        "// Generated by tools/content/import_alpha.py from the original SMAC alpha.txt.",
        "// Entries describe the complete base-game facility and secret-project catalog.",
        "const definitions = [",
    ]
    for entry in entries:
        output.extend(
            [
                "\t{",
                f"\t\tid: {quote(str(entry['id']))},",
                f"\t\tname: {quote(str(entry['name']))},",
                f"\t\tkind: {quote(str(entry['kind']))},",
                f"\t\tmineral_cost: {entry['mineral_cost']},",
                f"\t\tenergy_maintenance: {entry['energy_maintenance']},",
                f"\t\trequired_technology: {quote(str(entry['required_technology']))},",
                f"\t\tobsolete_technology: {quote(str(entry['obsolete_technology']))},",
                f"\t\teffect: {quote(str(entry['effect']))},",
                "\t},",
            ]
        )
    output.extend(["];", "", "return definitions;", ""])
    return "\n".join(output)


def technology_reference(
    code: str,
    code_to_id: dict[str, str],
) -> tuple[str, str]:
    if code == "None":
        return "always", ""
    if code == "Disable":
        return "disabled", ""
    try:
        return "technology", code_to_id[code]
    except KeyError as exc:
        raise ValueError(f"missing technology code {code}") from exc


def generate_unit_catalog(
    catalog: tuple[
        list[ChassisRow],
        list[ReactorRow],
        list[WeaponRow],
        list[ArmorRow],
        list[AbilityRow],
        list[UnitRow],
    ],
    technologies: list[TechnologyRow],
) -> str:
    chassis, reactors, weapons, armors, abilities, units = catalog
    code_to_id = {
        technology.code: canonical_id(technology.name)
        for technology in technologies
    }
    triads = {0: "land", 1: "sea", 2: "air"}

    def technology_fields(code: str) -> list[str]:
        availability, technology_id = technology_reference(code, code_to_id)
        return [
            f"\t\tavailability: {quote(availability)},",
            f"\t\trequired_technology: {quote(technology_id)},",
        ]

    output = [
        "// Generated by tools/content/import_alpha.py from the original SMAC alpha.txt.",
        "// Entries describe the complete base-game unit design catalog.",
    ]

    def begin_collection(name: str) -> None:
        output.extend(["", f"const {name} = ["])

    def end_collection() -> None:
        output.append("];")

    begin_collection("chassis")
    for entry in chassis:
        if entry.triad not in triads:
            raise ValueError(f"{entry.name} has invalid triad {entry.triad}")
        output.extend(
            [
                "\t{",
                f"\t\tid: {quote(canonical_id(entry.name))},",
                f"\t\tname: {quote(entry.name)},",
                f"\t\tspeed: {entry.speed},",
                f"\t\ttriad: {quote(triads[entry.triad])},",
                f"\t\trange: {entry.range},",
                f"\t\tmissile: {'true' if entry.missile else 'false'},",
                f"\t\tcargo: {entry.cargo},",
                f"\t\tcost: {entry.cost},",
                *technology_fields(entry.prerequisite_code),
                "\t},",
            ]
        )
    end_collection()

    begin_collection("reactors")
    for entry in reactors:
        output.extend(
            [
                "\t{",
                f"\t\tid: {quote(canonical_id(entry.name))},",
                f"\t\tname: {quote(entry.name)},",
                f"\t\tpower: {entry.power},",
                *technology_fields(entry.prerequisite_code),
                "\t},",
            ]
        )
    end_collection()

    begin_collection("weapons")
    for entry in weapons:
        output.extend(
            [
                "\t{",
                f"\t\tid: {quote(canonical_id(entry.name))},",
                f"\t\tname: {quote(entry.name)},",
                f"\t\tshort_name: {quote(entry.short_name)},",
                f"\t\toffense: {entry.offense},",
                f"\t\tmode: {entry.mode},",
                f"\t\tcost: {entry.cost},",
                f"\t\ticon: {entry.icon},",
                *technology_fields(entry.prerequisite_code),
                "\t},",
            ]
        )
    end_collection()

    begin_collection("armors")
    for entry in armors:
        output.extend(
            [
                "\t{",
                f"\t\tid: {quote(canonical_id(entry.name))},",
                f"\t\tname: {quote(entry.name)},",
                f"\t\tshort_name: {quote(entry.short_name)},",
                f"\t\tdefense: {entry.defense},",
                f"\t\tmode: {entry.mode},",
                f"\t\tcost: {entry.cost},",
                *technology_fields(entry.prerequisite_code),
                "\t},",
            ]
        )
    end_collection()

    begin_collection("abilities")
    for entry in abilities:
        output.extend(
            [
                "\t{",
                f"\t\tid: {quote(canonical_id(entry.name))},",
                f"\t\tname: {quote(entry.name)},",
                f"\t\tcost: {entry.cost},",
                f"\t\tabbreviation: {quote(entry.abbreviation)},",
                f"\t\tflags: {quote(entry.flags)},",
                f"\t\teffect: {quote(entry.effect)},",
                *technology_fields(entry.prerequisite_code),
                "\t},",
            ]
        )
    end_collection()

    chassis_ids = {entry.name: canonical_id(entry.name) for entry in chassis}
    weapon_ids = {entry.short_name: canonical_id(entry.name) for entry in weapons}
    armor_ids = {entry.short_name: canonical_id(entry.name) for entry in armors}
    begin_collection("predefined_units")
    for entry in units:
        try:
            chassis_id = chassis_ids[entry.chassis_name]
            weapon_id = weapon_ids[entry.weapon_name]
            armor_id = armor_ids[entry.armor_name]
        except KeyError as exc:
            raise ValueError(
                f"{entry.name} references missing unit component {exc.args[0]}"
            ) from exc
        availability, technology_id = technology_reference(
            entry.prerequisite_code,
            code_to_id,
        )
        if entry.hidden:
            availability = "disabled"
            technology_id = ""
        output.extend(
            [
                "\t{",
                f"\t\tid: {quote(canonical_id(entry.name))},",
                f"\t\tname: {quote(entry.name)},",
                f"\t\tchassis: {quote(chassis_id)},",
                f"\t\tweapon: {quote(weapon_id)},",
                f"\t\tarmor: {quote(armor_id)},",
                f"\t\tplan: {entry.plan},",
                f"\t\tmineral_cost: {entry.mineral_cost},",
                f"\t\tcargo: {entry.cargo},",
                f"\t\ticon: {entry.icon},",
                f"\t\tability_flags: {quote(entry.ability_flags)},",
                f"\t\tavailability: {quote(availability)},",
                f"\t\trequired_technology: {quote(technology_id)},",
                "\t},",
            ]
        )
    end_collection()
    output.extend(
        [
            "",
            "return {",
            "\tchassis: chassis,",
            "\treactors: reactors,",
            "\tweapons: weapons,",
            "\tarmors: armors,",
            "\tabilities: abilities,",
            "\tpredefined_units: predefined_units,",
            "};",
            "",
        ]
    )
    return "\n".join(output)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--alpha", type=Path, required=True)
    parser.add_argument("--technologies-output", type=Path, required=True)
    parser.add_argument("--facilities-output", type=Path)
    parser.add_argument("--units-output", type=Path)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    rows = read_technologies(args.alpha)
    output = generate_technology_catalog(rows)
    args.technologies_output.parent.mkdir(parents=True, exist_ok=True)
    args.technologies_output.write_text(output, encoding="ascii", newline="\n")
    print(f"wrote {len(rows)} technologies to {args.technologies_output}")
    if args.facilities_output is not None:
        facilities = read_facilities(args.alpha)
        facility_output = generate_facility_catalog(facilities, rows)
        args.facilities_output.parent.mkdir(parents=True, exist_ok=True)
        args.facilities_output.write_text(
            facility_output,
            encoding="ascii",
            newline="\n",
        )
        facility_count = sum(facility.kind == "facility" for facility in facilities)
        project_count = sum(facility.kind == "project" for facility in facilities)
        print(
            f"wrote {facility_count} facilities and {project_count} projects "
            f"to {args.facilities_output}"
        )
    if args.units_output is not None:
        unit_catalog = read_unit_catalog(args.alpha)
        unit_output = generate_unit_catalog(unit_catalog, rows)
        args.units_output.parent.mkdir(parents=True, exist_ok=True)
        args.units_output.write_text(unit_output, encoding="ascii", newline="\n")
        counts = [len(entries) for entries in unit_catalog]
        print(
            "wrote unit catalog "
            f"(chassis={counts[0]}, reactors={counts[1]}, weapons={counts[2]}, "
            f"armors={counts[3]}, abilities={counts[4]}, units={counts[5]}) "
            f"to {args.units_output}"
        )


if __name__ == "__main__":
    main()
