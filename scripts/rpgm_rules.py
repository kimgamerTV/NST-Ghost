import re

def extract_dependencies(condition_string):
    """
    Parses an RPG Maker condition string and returns a list of dependencies.
    Returns a list of tuples: (type, id_or_name)
    Types: 'switch', 'variable', 'actor', 'item', 'weapon', 'armor', 'state'
    """
    dependencies = []
    
    # 1. Switches: s[x] or s[x] == true/false
    # Regex for s[digits]
    switch_matches = re.finditer(r's\[(\d+)\]', condition_string)
    for match in switch_matches:
        switch_id = int(match.group(1))
        dependencies.append(('switch', switch_id))
        
    # 2. Variables: v[x]
    variable_matches = re.finditer(r'v\[(\d+)\]', condition_string)
    for match in variable_matches:
        variable_id = int(match.group(1))
        dependencies.append(('variable', variable_id))
        
    # 3. Actors: $gameActors.actor(x)
    actor_matches = re.finditer(r'\$gameActors\.actor\((\d+)\)', condition_string)
    for match in actor_matches:
        actor_id = int(match.group(1))
        dependencies.append(('actor', actor_id))
        
    # 4. Party Members: $gameParty.members().contains($gameActors.actor(x))
    # Covered by #3 mostly, but check for specific party checks if needed
    
    # 5. Items in inventory: $gameParty.hasItem($dataItems[x])
    item_matches = re.finditer(r'\$dataItems\[(\d+)\]', condition_string)
    for match in item_matches:
        item_id = int(match.group(1))
        dependencies.append(('item', item_id))

    # 6. Weapons: $dataWeapons[x]
    weapon_matches = re.finditer(r'\$dataWeapons\[(\d+)\]', condition_string)
    for match in weapon_matches:
        weapon_id = int(match.group(1))
        dependencies.append(('weapon', weapon_id))

    # 7. Armors: $dataArmors[x]
    armor_matches = re.finditer(r'\$dataArmors\[(\d+)\]', condition_string)
    for match in armor_matches:
        armor_id = int(match.group(1))
        dependencies.append(('armor', armor_id))
        
    # 8. States: isStateAffected(x)
    state_matches = re.finditer(r'isStateAffected\((\d+)\)', condition_string)
    for match in state_matches:
        state_id = int(match.group(1))
        dependencies.append(('state', state_id))

    return dependencies
