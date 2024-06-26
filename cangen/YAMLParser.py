from ruamel.yaml import YAML
from cangen.CANMsg import CANMsg, EncodableCANMsg
from cangen.CANField import CANPoint, NetField
from cangen.Format import Format
from cangen.Messages import Messages

MAX_MSG_SIZE = 8    # Note: Max size for CAN 2.0, change if FDCAN

class YAMLParser:
    '''
    A class to parse a given YAML string or file. Most of the heavy lifting
    is done by the internals of ruamel.yaml.
    '''

    def __init__(self):
        self.yaml = YAML(typ="safe")
        self.yaml.register_class(Messages)
        self.yaml.register_class(CANMsg)
        self.yaml.register_class(EncodableCANMsg)
        self.yaml.register_class(NetField)
        self.yaml.register_class(CANPoint)

    def parse(self, file) -> Messages:
        msgs : Messages = self.yaml.load(file)
        return msgs
