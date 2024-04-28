from __future__ import annotations
from ruamel.yaml import Optional
from dataclasses import dataclass

@dataclass
class CANField:
    '''
    Represents a field in a CAN message. Has an id, a name, a unit, a size,
    and an optional Format and encodings. Also knows its own
    index within its parent CANMsg, which is assigned at load from YAML.

    Think 1 MQTT Topic per 1 CAN Field
    '''

    name: str
    unit: str
    size: int # in bits
    signed : bool = False
    endianness : str = "little"
    field_type: str = "*"*42
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

    num_points: int = 0
    field_type = "composite"
