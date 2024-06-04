from dataclasses import dataclass

@dataclass
class Format:
   '''
   Represents a format to be applied to data after decoding.
   '''
   repr: str = ""

@dataclass
class Divide10(Format):
      repr: str = "divide10"

@dataclass
class Divide100(Format):
      repr: str = "divide100"

@dataclass
class Divide10000(Format):
      repr: str = "divide10000"

@dataclass
class Acceleration(Format):
      repr: str = "acceleration"

