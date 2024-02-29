from __future__ import annotations
from .Decoding import *
from ruamel.yaml import Optional
from dataclasses import dataclass

@dataclass
class CANField:
    '''
    Represents a field in a CAN message. Has an id, a name, a unit, a size,
    and an optional Format and Decodings. Also knows its own
    index within its parent CANMsg, which is assigned at load from YAML.
    '''
    name: str
    unit: str
    size: int
    index: int = -1
    final_type: str = "f32"
    decodings: Optional[list[Decoding]] = None
    format: Optional[str] = None
