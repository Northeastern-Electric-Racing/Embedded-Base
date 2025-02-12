from __future__ import annotations
from typing import Optional
from dataclasses import dataclass
import math

@dataclass 
class CANPoint:
    '''
    Represents one set of bits in a CAN message.
    Seperates CAN decoding logic from MQTT encoding information.
    '''
    size: int # in bits
    signed : bool = False
    endianness : str = "big"
    final_type: str = "f32"
    format: Optional[str] = None
    default: Optional[float] = 0

    def get_size_bytes(self):
        # Calculate max number of bytes to represent value
        num_bytes = self.size / 8
        if (self.size % 8):
            num_bytes += 1

        return int(num_bytes)
    
    def get_size_bits(self):
        return self.size
    
    def get_size_min_bytes(self):
        return math.ceil(self.size / 8.0) * 8 
    
@dataclass
class NetField:
    '''
    Represents a field in a CAN message.  Contains a MQTT name and unit.

    Think 1 MQTT Topic per 1 Net Field
    '''

    name: str
    unit: str
    points: list[CANPoint]
    send: bool = True
    topic_append: bool = False
    sim_min: Optional[float] = -1.0
    sim_max: Optional[float] = -1.0
    sim_inc_min: Optional[float] = -1.0
    sim_inc_max: Optional[float] = -1.0
