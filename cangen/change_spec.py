import json
import os
from pathlib import Path
from typing import List, Dict, Optional

CANGEN_PATH = "./can-messages"

# Convert a directory of JSON spec files
def convert_dir(d):
    dpath = Path(d)
    for fpath in dpath.iterdir():
        if fpath.is_file():
            new_msgs = []
            with open(fpath, 'r') as f:
                new_msgs = convert_file(f)
            new_fpath = fpath.with_name(f"{fpath.stem}.new{fpath.suffix}")
            with open(new_fpath, 'w') as f:
                f.write(json.dumps(new_msgs, indent=2))


# Convert a JSON spec file for all its messages
def convert_file(f):
    try:
        old_msgs = json.load(f)
        new_msgs = []
        for old_msg in old_msgs:
            new_msgs.append(convert_msg(old_msg))
        return new_msgs
    except json.JSONDecodeError as e:
        print(f"Error decoding {fpath}: {e}")

# Take a message in the old format, and convert it to the new format
# Return the message in the new format
def convert_msg(old_msg):
    old_fields = old_msg["fields"] 
    new_fields = []
    new_points = []
    points_idx = 1

    # Convert old fields, points into new 
    for old_field in old_fields:
        new_field_values = []

        for old_point in old_field["points"]:
            # Mandatory members
            new_point = {
                "size": old_point["size"]
            }
            
            # Optional members
            optional_keys = ["signed", "endianness", "format", "default", "ieee754_f32"]
            for key in optional_keys:
                if key in old_point: 
                    new_point[key] = old_point[key]
            if "sim" in old_field:
                new_point["sim"] = old_field["sim"]
            if "send" in old_field:
                new_point["parse"] = old_field["send"]

            # Append new point, update new field values, increment index
            new_points.append(new_point)
            new_field_values.append(points_idx)
            points_idx += 1

        new_field = {
            "name": old_field["name"],
            "unit": old_field["unit"],
            "values": new_field_values
        }
        new_fields.append(new_field)

    # Mandatory members
    new_msg = {
        "id": old_msg["id"],
        "desc": old_msg["desc"],
        "points": new_points,
        "fields": new_fields,
    }

    # Optional members
    optional_keys = ["key", "is_ext", "sim_freq"]
    for key in optional_keys:
        if key in old_msg:
            new_msg[key] = old_msg[key]
    
    return new_msg

# Entrypoint
if __name__ == "__main__":
    convert_dir(CANGEN_PATH)
