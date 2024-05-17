from __future__ import annotations
from typing import Optional
from dataclasses import dataclass

@dataclass(kw_only=True) 
class CANPoint():
    '''
    Represents one set of bits in a CAN message.
    Seperates CAN decoding logic from MQTT encoding information.

    CANField, CompositeField, and DiscreteField all derive from this.
    This allows CANUnit to define decoding properties and CANField to define encoding properties, 
    but keep them in one list for the yaml prettiness.
    '''
    size: int # in bits
    signed : bool = False
    endianness : str = "big"
    final_type: str = "f32"
    format: Optional[str] = None

    def get_size_bytes(self):
        # Calculate max number of bytes to represent value
        num_bytes = self.size / 8
        if (self.size % 8):
            num_bytes += 1

        return int(num_bytes)
    
    def get_size_bits(self):
        return self.size
    
@dataclass
class CANField(CANPoint):
    '''
    Represents a field in a CAN message.  Contains a MQTT name and unit.
    Differs from CANUnit as it assigns a MQTT message to one unit of can data.

    Think 1 MQTT Topic per 1 CAN Field
    '''

    name: str
    unit: str
    field_type: str = "*"*42
    send: bool = True
    

@dataclass
class DiscreteField(CANField):
    """
    Represents a discrete field inside a CAN message in the sense that
    it will make sense if it is published to MQTT and timestamped on its own
    (think State of Charge or Temperature)
    """

    field_type = "discrete"

@dataclass
class CompositeField(CANField):
    """
    Represents a composite field inside a CAN message in the sense that
    it will only make sense with other data points in the same message and
    can't be timestamped on its own (think XYZ acceleration or Roll, Pitch, Yaw)
    """

    points = list[CANPoint]
    field_type = "composite"
