from __future__ import annotations
from dataclasses import dataclass
from ruamel.yaml import Optional
from enum import Enum
from .CANField import CANField

class CANMsgType(Enum):
    BROADCAST = 1
    COMMAND = 2
    UNDEFINED = 3

@dataclass
class CANMsg:
    '''
    Represents a CAN message. Has an id, a description, and a number of individual fields.
    '''
    id: str     # Hex value of CAN ID, i.e. `0x88`
    desc: str   # Brief name of CAN message, used for generating function names
    fields: list[CANField] # List of CAN fields in the message (Optional to allow for only networkEncoding)
    msg_type: CANMsgType = CANMsgType.UNDEFINED

    def __setstate__(self, state):
        self.__init__(**state)

@dataclass
class BroadcastMsg(CANMsg):
    """
    Represents a CAN Msg that is _broadcast from_ the node
    """

    msg_type = CANMsgType.BROADCAST

@dataclass
class CommandMsg(CANMsg):
    """
    Represents a CAN Msg that is _commanded to_ the node
    """

    msg_type = CANMsgType.COMMAND
