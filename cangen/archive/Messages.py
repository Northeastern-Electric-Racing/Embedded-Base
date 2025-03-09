from dataclasses import dataclass
from .CANMsg import CANMsg

@dataclass
class Messages:
      '''
      Represents a list of CAN messages, used to create a container for each YAML
      '''
      msgs: list[CANMsg]
