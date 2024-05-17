from __future__ import annotations
from dataclasses import dataclass
from .CANField import NetField

@dataclass
class CANMsg:
    '''
    Represents a CAN message. Has an id, a description, and a number of individual fields.
    '''
    id: str     # Hex value of CAN ID, i.e. `0x88`
    desc: str   # Brief name of CAN message, used for generating function names
    fields: list[NetField] # List of CAN fields in the message (Optional to allow for only networkEncoding)

    def __setstate__(self, state):
        self.__init__(**state)
