from dataclasses import dataclass
from .CANField import CANField
from ruamel.yaml import Optional

@dataclass
class NetworkEncoding:
    '''
    Determines the format of the data to be sent over the network. 
    '''
    id: str
    fields: list[CANField]
    topic: Optional[str] = None
    unit: Optional[str] = None

@dataclass
class CSV(NetworkEncoding):
    id = "csv"

@dataclass
class SinglePoint(NetworkEncoding):
    id = "single_point"
