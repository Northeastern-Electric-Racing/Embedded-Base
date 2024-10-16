from __future__ import annotations
from dataclasses import dataclass
from typing import Optional
from .CANField import NetField

@dataclass
class CANMsg:
    '''
    Represents a CAN message. Has an id, a description, and a number of individual fields.
    '''
    id: str     # Hex value of CAN ID, i.e. `0x88`
    desc: str   # Brief name of CAN message, used for generating function names
    fields: list[NetField] # List of CAN fields in the message
    sim_freq: Optional[int] # Frequency to simulate this message at, in ms

@dataclass
class EncodableCANMsg(CANMsg):
    '''
    Represents a CAN message that can also be encoded to using the command_data protobuf spec.
    Has all properties of a decodable message, but also has a key and optional is_ext field.

    IMPORTANT: Use the default flag in each CANPoint to specify a default value to be sent before a command is sent.
    '''
    '''
    The name to be used to look up command data over mqtt
    '''
    key: str
    '''
    Whether the CAN ID is extended or standard
    '''
    is_ext: Optional[bool] = False
