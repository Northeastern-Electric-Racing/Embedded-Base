from ruamel.yaml import YAML, Any
from cangen.CANMsg import CANMsg
from cangen.CANField import CANField
from cangen.Format import Format
from cangen.Decoding import Decoding
from cangen.Messages import Messages
from cangen.NetworkEncoding import NetworkEncoding

class YAMLParser:
    '''
    A class to parse a given YAML string or file. Most of the heavy lifting
    is done by the internals of ruamel.yaml.
    '''

    def __init__(self):
        self.yaml = YAML()
        self.yaml.register_class(Messages)
        self.yaml.register_class(CANMsg)
        self.yaml.register_class(CANField)
        for encoding in NetworkEncoding.__subclasses__():
            self.yaml.register_class(encoding)
        for decoding in Decoding.__subclasses__():
            self.yaml.register_class(decoding)

    def parse(self, file: Any) -> Messages:
        return self.yaml.load(file)
