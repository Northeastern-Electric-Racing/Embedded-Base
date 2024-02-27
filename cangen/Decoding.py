from dataclasses import dataclass

@dataclass
class Decoding:
    '''
    This is an abstract class (well, what passes for one in Python)
    that represents a decoding to be applied to a slice of data.
    '''
    bits: int
    entry_type: str
    repr: str = "*"*42

@dataclass
class BigEndian(Decoding):
    repr: str = "big_endian"
    entry_type = "&[u8]"

@dataclass
class LittleEndian(Decoding):
    repr: str = "little_endian"
    entry_type = "&[u8]"

@dataclass
class TwosComplement(Decoding):
    repr: str = "twos_comp"
    entry_type = "u32"


@dataclass
class Half(Decoding):
    repr: str = "half"
    entry_type = "u8"
