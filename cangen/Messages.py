from dataclasses import dataclass
from .CANMsg import CANMsg

@dataclass
class Messages:
      '''
      Represents a list of CAN messages. Has a list of CANMsgs.
      '''
      msgs: list[CANMsg]
